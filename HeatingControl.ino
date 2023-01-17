#include "src/lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include <Arduino.h>

#include "global.h"

#include "src/Common_code/Network/network.h"
#include "src/Common_code/Network/TcpServer.h"
#include "src/Common_code/Network/HttpServer.h"
#include "src/Common_code/Network/TelnetServer.h"
#include "src/Common_code/Network/servlets/servlets.h"
#include "src/Common_code/Network/servlets/tOutputControlServlets.h"
#include "src/Common_code/Network/servlets/tSensorStateServlet.h"
#include "src/tHeatingControlServletTMP.h"


#include "src/Common_code/WatchdogProcess.h"

#include "src/Common_code/sensors/tSensor.h"
#include "src/Common_code/sensors/tSimpleDigitalInputSensor.h"
#include "src/Common_code/sensors/tDS1820Sensor.h"
#include "src/Common_code/sensors/tImpulseSensor.h"
#include "src/Common_code/sensors/tPt100AnalogSensor.h"
#include "src/Common_code/sensors/tSensorHub.h"
#include "src/Common_code/sensors/tHeatingCircleStatusSensor.h"
#include "src/Common_code/sensors/tOutputStateSensor.h"
#include "src/Common_code/sensors/tSimpleDigitalInputSensor.h"
#include "src/Common_code/sensors/tSystemStatusSensor.h"
#include "src/tOutputProcessheatingControl.h"
#include "src/tOutputProcessheatingControl.h"
#include "src/Common_code/controllers/tHeatingCircleControl.h"

// restart if no connection for 5 minutes
#define TCP_WATCHDOG_TIMEOUT 300 

Scheduler sched;
tSensorProcess SensorProcess(sched); 
//WorkerProcess Worker(sched);
tOutputProcess_heatingControl OutputProcess(sched);
tWatchdogProcess WatchdogProcess(sched);

#if CONFIG_NETWORK

tNetwork Network;
tTcpServerProcess TcpServerProcess(sched, TCP_WATCHDOG_TIMEOUT);

#if CONFIG_HTTP_SERVER
tHttpServer HttpServer;
#endif

#if CONFIG_TELNET_SERVER
extern tTelnetServer TelnetServer;
#endif

#if CONFIG_HTTP_SERVER
tHttpServlet * ServletFactory(String *pRequestBuffer)
{
   if (pRequestBuffer->startsWith("/OutputControl.js")) return new tOutputControlJavaScript();
   if (pRequestBuffer->startsWith("/outputState")) return new tOutputStateServlet(&OutputProcess);
   if (pRequestBuffer->startsWith("/outputSet")) return new tOutputSetServlet(&OutputProcess);
   if (pRequestBuffer->startsWith("/sensorState")) return new tSensorStateServlet();
   if (pRequestBuffer->startsWith("/heat")) return new tHeatingControlServletTMP();


   return NULL;
}
#endif

#endif // CONFIG_NETWORK

class tDS1820SensorCallback : public tSensorHubEvent
{
public:
   tDS1820SensorCallback() {};
   
   virtual void onEvent(uint8_t SensorID, tSensorEventType EventType, uint8_t dataBlobSize, void *pDataBlob)
   {
      tDS1820Sensor::tResult *pDS1820Result =(tDS1820Sensor::tResult *) pDataBlob;
      
      switch (EventType)
      {
         case EV_TYPE_MEASUREMENT_COMPLETED: 
            DEBUG_PRINT_3("Measurement completed. devs: ");
            DEBUG_3(print(pDS1820Result->NumOfDevices));
            DEBUG_PRINT_3(" Avg: ");
            DEBUG_3(print(pDS1820Result->Avg));
            uint8_t NumOfItems;
            if (pDS1820Result->Avg) {
               NumOfItems = 1;
            }
            else
            {
               NumOfItems = pDS1820Result->NumOfDevices;
            }
            for (int i = 0; i < NumOfItems; i++)
            {
               DEBUG_PRINT_3(" dev: ");
               DEBUG_3(print(i));
               DEBUG_PRINT_3(" temp: ");
               DEBUG_3(print(((float)(pDS1820Result)->Dev[i].Temperature) / 10));
            }
            DEBUG_PRINTLN_3("");
            break;
   
         case EV_TYPE_MEASUREMENT_ERROR:
            DEBUG_PRINTLN_3("Measurement completed. ERROR");
            break;
      }
   }
};

class tImpulseSensorCallback : public tSensorHubEvent
{
public:
   tImpulseSensorCallback() {};
   virtual void onEvent(uint8_t SensorID, tSensorEventType EventType, uint8_t dataBlobSize, void *pDataBlob)
   {
      if (EV_TYPE_MEASUREMENT_COMPLETED == EventType)
      {
         tImpulseSensor::tResult *pResult =(tImpulseSensor::tResult *) pDataBlob;
         DEBUG_PRINT_3("SensorID: ");
         DEBUG_3(print(SensorID));
         DEBUG_PRINT_3(" impulse count: "); 
         DEBUG_3(print(pResult->Count));
         DEBUG_PRINT_3(" Sum: "); 
         DEBUG_3(println(pResult->Sum));
      }
   }
};

class tPt100SensorCallback : public tSensorHubEvent
{
public:
   tPt100SensorCallback() {};
   virtual void onEvent(uint8_t SensorID, tSensorEventType EventType, uint8_t dataBlobSize, void *pDataBlob)
   {
      if (EV_TYPE_MEASUREMENT_COMPLETED == EventType)
      {
         tPt100AnalogSensor::tResult *pResult =(tPt100AnalogSensor::tResult *) pDataBlob;
         DEBUG_PRINT_3("PT100 Temp: "); 
         DEBUG_3(println(pResult->Temperature));
      }
   }
};

tPt100SensorCallback Pt100SensorCallback;
tDS1820SensorCallback DS1820SensorCallback;
tImpulseSensorCallback ImpulseSensorCallback;

tDS1820Sensor::DeviceAddress FloorTemperatureTempSensorSerial = { 0x28, 0x3C, 0x1F, 0x5F, 0xA1, 0x21, 0x01, 0xD9};
tDS1820Sensor::DeviceAddress RadiatorsTemperatureTempSensorSerial = { 0x28, 0x44, 0x9B, 0x80, 0xA1, 0x21, 0x01, 0xAF};
tDS1820Sensor::DeviceAddress HeatingSourceTempSensorSerial = { 0x28, 0x44, 0x9B, 0x80, 0xA1, 0x21, 0x01, 0xAF};
tDS1820Sensor::DeviceAddress HeatingStorageTempSensorSerial = { 0x28, 0x6D, 0xDB, 0x7E, 0xA1, 0x21, 0x01, 0x02};

tHeatingCircleControl *pFloorTemperatureValveControl; 
tHeatingCircleControl *pRadiatorsTemperatureValveControl; 

tImpulseSensor *pImpulseSensor = NULL;
//tImpulseSensor *pImpulseSensor1 = NULL;

void Interrupt(void)
{
   if(pImpulseSensor) pImpulseSensor->Impulse();
}

//void Interrupt1(void)
//{
//   if(pImpulseSensor1) pImpulseSensor1->Impulse();
//}


tSensorHub SensorHub;

void setup() {
  if (EEPROM.read(EEPROM_CANNARY_OFFSET) != EEPROM_CANNARY)
    SetDefaultEEPromValues();

#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.begin(115200);
  while (!DEBUG_SERIAL);
  DEBUG_SERIAL.print(F("START, v"));
  DEBUG_SERIAL.println(FW_VERSION);
#endif

#if CONFIG_NETWORK
  Network.init();
  TcpServerProcess.add(true);
#endif // CONFIG_NETWORK
  
#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.println(F("START Tcp "));
#endif

  SensorProcess.add(true);
//  Worker.add();
  OutputProcess.add(true);
  WatchdogProcess.add(true);


#define SENSOR_ID_SYSTEM_STATUS 1
#define SENSOR_ID_1820_HEATING_TEMP 2
#define SENSOR_ID_1820_AIR_HUW_TEMP 3
#define SENSOR_ID_1820_OUTDOOR_TEMP 4
  
#define SENSOR_ID_IMPULSE_HEATPUMP        5
#define SENSOR_ID_IMPULSE_WATER_HEATER    6
  
#define SENSOR_ID_PT100_HOTAIR            7 
  
#define SENSOR_ID_DIGITAL_RADIATORS_PUMP_CONTROL   8
#define SENSOR_ID_DIGITAL_WATER_HEATER_REQUEST     9
#define SENSOR_ID_DIGITAL_AUX                      10

#define SENSOR_ID_OUTPUT_STATES                    11

#define SENSOR_ID_RADIATORS_HEATING_STATE          12
#define SENSOR_ID_FLOOR_HEATING_STATE              13

  
  tSensor *pSensor;

  pSensor = new tSystemStatusSensor;
  pSensor->Register(SENSOR_ID_SYSTEM_STATUS,"SystemStatus",NULL,10); //1 sec

  tDS1820Sensor::tConfig Ds1820Config;
  
  Ds1820Config.Pin = 2;
  Ds1820Config.Avg = 0;
  pSensor = new tDS1820Sensor;
  pSensor->Register(SENSOR_ID_1820_HEATING_TEMP,"HeatingTemp",&Ds1820Config,50); //5 sec
  
  
  Ds1820Config.Pin = 3;
  Ds1820Config.Avg = 0;
  pSensor = new tDS1820Sensor;
  pSensor->Register(SENSOR_ID_1820_AIR_HUW_TEMP,"AirHuwTemp",&Ds1820Config,50); //5 sec

  Ds1820Config.Pin = 7;
  Ds1820Config.Avg = 0;
  pSensor = new tDS1820Sensor;
  pSensor->Register(SENSOR_ID_1820_OUTDOOR_TEMP,"OutdoorTemp",&Ds1820Config,50); //5 sec
  
  
  pFloorTemperatureValveControl = new tHeatingCircleControl(
        FloorTemperatureTempSensorSerial,
        HeatingSourceTempSensorSerial,
        HeatingStorageTempSensorSerial,
        OUT_ID_FLOOR_TEMP_HIGHER,
        OUT_ID_FLOOR_TEMP_LOWER,
        OUT_ID_FLOOR_PUMP,
        2); 
  pRadiatorsTemperatureValveControl = new tHeatingCircleControl(
        RadiatorsTemperatureTempSensorSerial,
        HeatingSourceTempSensorSerial,
        HeatingStorageTempSensorSerial,
        OUT_ID_RADIATOR_TEMP_HIGHER,
        OUT_ID_RADIATOR_TEMP_LOWER,
        OUT_ID_READIATORS_PUMP,
        2); 


  pFloorTemperatureValveControl->setTargetTemp(28);
  pFloorTemperatureValveControl->setTolerance(0.7);
  pFloorTemperatureValveControl->setHisteresis(0.8);
  pFloorTemperatureValveControl->setFastThold(1.5); 

  pRadiatorsTemperatureValveControl->setTargetTemp(31);
  pRadiatorsTemperatureValveControl->setTolerance(0.7);
  pRadiatorsTemperatureValveControl->setHisteresis(1);
  pRadiatorsTemperatureValveControl->setFastThold(2); 

  SensorHub.subscribeToEvents(SENSOR_ID_1820_HEATING_TEMP,pFloorTemperatureValveControl);
  SensorHub.subscribeToEvents(SENSOR_ID_1820_HEATING_TEMP,pRadiatorsTemperatureValveControl);
  SensorHub.subscribeToEvents(SENSOR_ID_1820_OUTDOOR_TEMP,&DS1820SensorCallback);

  tSimpleDigitalInputSensor::tConfig DigitalInputSensorConfig;

  DigitalInputSensorConfig.ActiveState = 0;
  DigitalInputSensorConfig.Pin = A9;
  pSensor = new tSimpleDigitalInputSensor;
  pSensor->Register(SENSOR_ID_DIGITAL_WATER_HEATER_REQUEST,"WaterHeaterRequest",&DigitalInputSensorConfig,10); //1 sec

  DigitalInputSensorConfig.ActiveState = 0;
  DigitalInputSensorConfig.Pin = A8;
  pSensor = new tSimpleDigitalInputSensor;
  pSensor->Register(SENSOR_ID_DIGITAL_RADIATORS_PUMP_CONTROL,"PumpControl",&DigitalInputSensorConfig,1); //1 sec

  DigitalInputSensorConfig.ActiveState = 0;
  DigitalInputSensorConfig.Pin = A7;
  pSensor = new tSimpleDigitalInputSensor;
  pSensor->Register(SENSOR_ID_DIGITAL_AUX,"DigitalInputAux",&DigitalInputSensorConfig,1); //1 sec
  

  pImpulseSensor = new tImpulseSensor;
  pImpulseSensor->Register(SENSOR_ID_IMPULSE_HEATPUMP,"HeatPumpEnergy",NULL,50); //5 sec
  pinMode(21, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(21), Interrupt, FALLING);
//  pImpulseSensor1 = new tImpulseSensor;
//  pImpulseSensor1->Register(SENSOR_ID_IMPULSE_WATER_HEATER,"AuxHeatEnergy",NULL,50); //5 sec
//  pinMode(20, INPUT_PULLUP);
//  attachInterrupt(digitalPinToInterrupt(20), Interrupt1, FALLING);

  tPt100AnalogSensor::tConfig Pt100AnalogSensorConfig;
  Pt100AnalogSensorConfig.Pin = A14;
  Pt100AnalogSensorConfig.Correction = 8;
  pSensor = new tPt100AnalogSensor;
  pSensor->Register(SENSOR_ID_PT100_HOTAIR,"HotAir",&Pt100AnalogSensorConfig,50); //5 sec

  
  pSensor = new tOutputStateSensor;
  pSensor->Register(SENSOR_ID_OUTPUT_STATES,"OutStates",&Pt100AnalogSensorConfig,50); //5 sec
  
  tHeatingCircleStatusSensor::tConfig HeatingCircleStatusSensorConfig;

  HeatingCircleStatusSensorConfig.pHeatingControl = pRadiatorsTemperatureValveControl;
  pSensor = new tHeatingCircleStatusSensor;
  pSensor->Register(SENSOR_ID_RADIATORS_HEATING_STATE,"RadiatorsState",&HeatingCircleStatusSensorConfig,50); //5 sec
  
  HeatingCircleStatusSensorConfig.pHeatingControl = pFloorTemperatureValveControl;
  pSensor = new tHeatingCircleStatusSensor;
  pSensor->Register(SENSOR_ID_FLOOR_HEATING_STATE,"FloorState",&HeatingCircleStatusSensorConfig,50); //5 sec

#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.print(F("Free RAM: "));
  DEBUG_SERIAL.println(getFreeRam());
  DEBUG_SERIAL.println(F("SYSTEM INITIALIZED"));
#endif
}


void loop() {
  sched.run();
}
