#include "src/lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include <Arduino.h>

#include "global.h"

#include "src/Common_code/Network/network.h"
#include "src/Common_code/Network/TcpServer.h"
#include "src/Common_code/Network/HttpServer.h"
#include "src/Common_code/Network/TelnetServer.h"
#include "src/Common_code/Network/servlets.h"
#include "src/Common_code/WatchdogProcess.h"
#include "src/Common_code/sensors/tSensor.h"
#include "src/Common_code/sensors/tSimpleDigitalInputSensor.h"
#include "src/Common_code/sensors/tDS1820Sensor.h"
#include "src/Common_code/sensors/tImpulseSensor.h"
#include "src/Common_code/sensors/tPt100AnalogSensor.h"
#include "src/Common_code/sensors/tSensorStateServlet.h"
#include "src/Common_code/sensors/tSensorHub.h"
#include "src/Common_code/sensors/tHeatingCircleStatusSensor.h"
#include "src/tOutputProcessheatingControl.h"
#include "src/tOutputProcessheatingControl.h"
#include "src/Common_code/controllers/tHeatingCircleControl.h"
#include "src/servlets.h"

// restart if no connection for 5 minutes
#define TCP_WATCHDOG_TIMEOUT 300 

Scheduler sched;
tSensorProcess SensorProcess(sched); 
//WorkerProcess Worker(sched);
tOutputProcess_heatingControl OutputProcess(sched);
tWatchdogProcess WatchdogProcess(sched);

tNetwork Network;
tTcpServerProcess TcpServerProcess(sched, TCP_WATCHDOG_TIMEOUT);
tHttpServer HttpServer;
extern tTelnetServer TelnetServer;

tHttpServlet * ServletFactory(String *pRequestBuffer)
{
   if (pRequestBuffer->startsWith("/OutputControl.js")) return new tOutputControlJavaScript();
   if (pRequestBuffer->startsWith("/outputState")) return new tOutputStateServlet();
   if (pRequestBuffer->startsWith("/outputSet")) return new tOutputSetServlet();
   if (pRequestBuffer->startsWith("/sensorState")) return new tSensorStateServlet();
   if (pRequestBuffer->startsWith("/heat")) return new tHeatingControlServletTMP();


   return NULL;
}

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
         DEBUG_PRINT_3("Impulse count: "); 
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
tImpulseSensor *pImpulseSensor1 = NULL;

void Interrupt(void)
{
   if(pImpulseSensor) pImpulseSensor->Impulse();
}

void Interrupt1(void)
{
   if(pImpulseSensor1) pImpulseSensor1->Impulse();
}


tSensorHub SensorHub;

void setup() {
   DEBUG_SERIAL.begin(115200);
   while (!DEBUG_SERIAL);
   DEBUG_PRINT_3("START, v");
   DEBUG_PRINTLN_3(FW_VERSION);

  if (EEPROM.read(EEPROM_CANNARY_OFFSET) != EEPROM_CANNARY)
    SetDefaultEEPromValues();


  Network.init();
  TcpServerProcess.add(true);
  DEBUG_PRINTLN_3("START Tcp ");

  SensorProcess.add(true);
//  Worker.add();
  OutputProcess.add(true);
  WatchdogProcess.add(true);
  

#define SENSOR_ID_1820_HEATING_TEMP 1
#define SENSOR_ID_1820_AIR_HUW_TEMP 2
#define SENSOR_ID_1820_OUTDOOR_TEMP 3
  
#define SENSOR_ID_IMPULSE_HEATPUMP        4
#define SENSOR_ID_IMPULSE_WATER_HEATER    5
  
#define SENSOR_ID_PT100_HOTAIR            6 
  
#define SENSOR_ID_DIGITAL_RADIATORS_PUMP_CONTROL   7
#define SENSOR_ID_DIGITAL_WATER_HEATER_REQUEST     8
#define SENSOR_ID_DIGITAL_AUX                      9

#define SENSOR_ID_OUTPUT_STATES                    10

#define SENSOR_ID_RADIATORS_HEATING_STATE          11
#define SENSOR_ID_FLOOR_HEATING_STATE              12

  
  tDS1820Sensor::tConfig Ds1820Config;

  Ds1820Config.Pin = 2;
  Ds1820Config.Avg = 0;
  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_DS1820, SENSOR_ID_1820_HEATING_TEMP, "HeatingTemp", &Ds1820Config, 50); //5 sec

  Ds1820Config.Pin = 3;
  Ds1820Config.Avg = 0;
  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_DS1820, SENSOR_ID_1820_AIR_HUW_TEMP, "AirHuwTemp",  &Ds1820Config, 50); //5 sec
  
  Ds1820Config.Pin = 7;
  Ds1820Config.Avg = 0;
  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_DS1820, SENSOR_ID_1820_OUTDOOR_TEMP, "OutdoorTemp",  &Ds1820Config, 50); //5 sec

  
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

  pFloorTemperatureValveControl->Stop();
  pRadiatorsTemperatureValveControl->Stop();


  tSimpleDigitalInputSensor::tConfig DigitalInputSensorConfig;

  DigitalInputSensorConfig.ActiveState = 0;
  DigitalInputSensorConfig.Pin = A7;
  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_DIGITAL_INPUT, SENSOR_ID_DIGITAL_WATER_HEATER_REQUEST , "WaterHeaterRequest", &DigitalInputSensorConfig, 10);   // 1sec 
  
  DigitalInputSensorConfig.Pin = A8;
  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_DIGITAL_INPUT, SENSOR_ID_DIGITAL_RADIATORS_PUMP_CONTROL, "PumpControl", &DigitalInputSensorConfig, 10);   // 1sec 

  DigitalInputSensorConfig.Pin = A9;
  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_DIGITAL_INPUT, SENSOR_ID_DIGITAL_AUX, "DigitalInputAux", &DigitalInputSensorConfig, 10);   // 1sec 
  
//  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_IMPULSE, SENSOR_ID_IMPULSE, "HeatPumpPower", NULL, 50); //5 sec
//  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_IMPULSE, SENSOR_ID_IMPULSE1, "AuxHeatPower", NULL, 50); //5 sec

  // local pointers to sensors - interrupts
//  pImpulseSensor = tSensor::getSensor(SENSOR_ID_IMPULSE);
//  pImpulseSensor1 = tSensor::getSensor(SENSOR_ID_IMPULSE1);
//  pinMode(20, INPUT_PULLUP);
//  attachInterrupt(digitalPinToInterrupt(20), Interrupt, FALLING);
//  pinMode(21, INPUT_PULLUP);
//  attachInterrupt(digitalPinToInterrupt(21), Interrupt1, FALLING);
//  
//  
//  SensorHub.subscribeToEvents(SENSOR_ID_IMPULSE,&ImpulseSensorCallback);
//  SensorHub.subscribeToEvents(SENSOR_ID_IMPULSE1,&ImpulseSensorCallback);

//#define SENSOR_ID_PT100_ANALOG 
//#define SENSOR_ID_PT100_ANALOG1 
//
  tPt100AnalogSensor::tConfig Pt100AnalogSensorConfig;
  Pt100AnalogSensorConfig.Pin = A14;
  Pt100AnalogSensorConfig.Correction = 8;
  
  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_PT100_ANALOG, SENSOR_ID_PT100_HOTAIR, "HotAir", &Pt100AnalogSensorConfig, 100); //10 sec

//  SensorHub.subscribeToEvents(SENSOR_ID_PT100_HOTAIR,&Pt100SensorCallback);
  
  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_OUTPUT_STATES, SENSOR_ID_OUTPUT_STATES, "OutStates", NULL, 10); // 1 sec

  tHeatingCircleStatusSensor::tConfig HeatingCircleStatusSensorConfig;

  HeatingCircleStatusSensorConfig.pHeatingControl = pRadiatorsTemperatureValveControl;
  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_HEATING_CIRCLE_STATE, SENSOR_ID_RADIATORS_HEATING_STATE, "RadiatorsState", &HeatingCircleStatusSensorConfig, 50); // 5 sec

  HeatingCircleStatusSensorConfig.pHeatingControl = pFloorTemperatureValveControl;
  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_HEATING_CIRCLE_STATE, SENSOR_ID_FLOOR_HEATING_STATE, "FloorState", &HeatingCircleStatusSensorConfig, 50); // 5 sec

  DEBUG_PRINT_3("Free RAM: ");
  DEBUG_3(println(getFreeRam()));
  DEBUG_3(println(F("SYSTEM INITIALIZED")));
}


void loop() {
  sched.run();
}
