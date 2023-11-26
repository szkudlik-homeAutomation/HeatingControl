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

#include "src/Common_code/sensors/tSensorFactory.h"
#include "src/Common_code/sensors/tSensor.h"
#include "src/Common_code/sensors/tSimpleDigitalInputSensor.h"
#include "src/Common_code/sensors/tDS1820Sensor.h"
#include "src/Common_code/sensors/tImpulseSensor.h"
#include "src/Common_code/sensors/tPt100AnalogSensor.h"
#include "src/Common_code/sensors/tSensorHub.h"
#include "src/sensors/tHeatingCircleStatusSensor.h"
#include "src/Common_code/sensors/tOutputStateSensor.h"
#include "src/Common_code/sensors/tSimpleDigitalInputSensor.h"
#include "src/Common_code/sensors/tSystemStatusSensor.h"
#include "src/tOutputProcessheatingControl.h"
#include "src/tOutputProcessheatingControl.h"
#include "src/tHeatingCircleControl.h"
#include "src/tHeatingCtrlIncomingFrameHandler.h"
#include "src/Common_code/TLE8457_serial/TLE8457_serial_lib.h"
#include "src/Common_code/WorkerProcess.h"

// restart if no connection for 5 minutes
#define TCP_WATCHDOG_TIMEOUT 300 

Scheduler sched;
tSensorProcess SensorProcess(sched); 
//WorkerProcess Worker(sched);
tOutputProcess_heatingControl OutputProcess(sched);
tWatchdogProcess WatchdogProcess(sched);

#if CONFIG_TLE8457_COMM_LIB
tHeatingCtrlIncomingFrameHandler IncomingFrameHandler;
CommRecieverProcess CommReciever(sched,EEPROM.read(EEPROM_DEVICE_ID_OFFSET));
CommSenderProcess CommSender(sched,EEPROM.read(EEPROM_DEVICE_ID_OFFSET),EEPROM.read(EEPROM_DEVICE_ID_OFFSET));

void COMM_SERIAL_EVENT() {
  CommReciever.serialEvent();
}
#endif //CONFIG_TLE8457_COMM_LIB

#if CONFIG_WORKER_PROCESS
WorkerProcess Worker(sched);
#endif

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
   if (pRequestBuffer->startsWith("/outputState")) return new tOutputStateServlet();
   if (pRequestBuffer->startsWith("/outputSet")) return new tOutputSetServlet();
#if CONFIG_SENSOR_STATE_SERVLET 
   if (pRequestBuffer->startsWith("/sensorState")) return new tSensorStateServlet();
#endif // CONFIG_SENSOR_STATE_SERVLET
   if (pRequestBuffer->startsWith("/heat")) return new tHeatingControlServletTMP();
   return NULL;
}
#endif

#endif // CONFIG_NETWORK

#if CONFIG_SENSOR_HUB
tSensorHub SensorHub;
#endif // CONFIG_SENSOR_HUB

class tHeatingConrolSensorFactory : public tSensorFactory
{
public:
	tHeatingConrolSensorFactory() : tSensorFactory() {}
protected:
   virtual tSensorDesc *appSpecificCreateDesc(uint8_t SensorType)
   {
        tSensorDesc *newSensorDesc = NULL;
        switch (SensorType)
        {
            case SENSOR_TYPE_HEATING_CIRCLE_STATE:
                newSensorDesc = new tHeatingCircleStatusSensorDesc();
                break;
        }

        return newSensorDesc;
   }

   virtual tSensor *appSpecificCreateSensor(uint8_t SensorType, uint8_t SensorID)
   {
	   tSensor *newSensor= NULL;
       switch (SensorType)
       {
           case SENSOR_TYPE_HEATING_CIRCLE_STATE:
        	   newSensor = new tHeatingCircleStatusSensor(SensorID);
               break;
       }

       return newSensor;
   }
};

tHeatingConrolSensorFactory SensorFactory;

class tDS1820SensorCallback : public tSensorHubEvent
{
public:
   tDS1820SensorCallback() {};
   
   virtual void onEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob)
   {
      tDS1820Sensor::tResult *pDS1820Result =(tDS1820Sensor::tResult *) pDataBlob;
      
      if (EventType & EV_TYPE_MEASUREMENT_COMPLETED)
      {
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
      }
   }
};

class tImpulseSensorCallback : public tSensorHubEvent
{
public:
   tImpulseSensorCallback() {};
   virtual void onEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob)
   {
      if (EV_TYPE_MEASUREMENT_COMPLETED & EventType)
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
   virtual void onEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob)
   {
      if (EV_TYPE_MEASUREMENT_COMPLETED & EventType)
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

void setup() {
#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.begin(115200);
  while (!DEBUG_SERIAL);
  DEBUG_SERIAL.print(F("START, v"));
  DEBUG_SERIAL.println(FW_VERSION);
#endif

  if (EEPROM.read(EEPROM_CANNARY_OFFSET) != EEPROM_CANNARY)
    SetDefaultEEPromValues();

  COMM_SERIAL.begin(9600);
  while (!COMM_SERIAL);

#if CONFIG_TLE8457_COMM_LIB
  CommSender.add();
  CommReciever.add();
#endif //CONFIG_TLE8457_COMM_LIB

#if CONFIG_NETWORK
  Network.init();
  TcpServerProcess.add(true);
#endif // CONFIG_NETWORK
  
#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.println(F("START Tcp "));
#endif

  SensorProcess.add(true);
#if CONFIG_WORKER_PROCESS
  Worker.add();
#endif //CONFIG_WORKER_PROCESS
  OutputProcess.add(true);
  WatchdogProcess.add(true);


#define SENSOR_ID_SYSTEM_STATUS 1
  tSensor *pSensor;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_SYSTEM_STATUS, SENSOR_ID_SYSTEM_STATUS,1,NULL,0,50,true);
  tSensorHub::Instance->RegisterLocalSensor(SENSOR_ID_SYSTEM_STATUS, "SystemStatus");

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

  tDS1820Sensor::tConfig DS1820config;
  tSimpleDigitalInputSensor::tConfig SimpleDigitalInputSensorConfig;
  tPt100AnalogSensor::tConfig Pt100AnalogSensorConfig;
  
  DS1820config.Avg = 0;
  DS1820config.Pin = 2;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_DS1820, SENSOR_ID_1820_HEATING_TEMP,1,&DS1820config,sizeof(DS1820config),50,true);	// 5sec
  tSensorHub::Instance->RegisterLocalSensor(SENSOR_ID_1820_HEATING_TEMP, "HeatingTemp");
  
  DS1820config.Avg = 0;
  DS1820config.Pin = 2;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_DS1820, SENSOR_ID_1820_AIR_HUW_TEMP,1,&DS1820config,sizeof(DS1820config),50,true);	// 5sec
  tSensorHub::Instance->RegisterLocalSensor(SENSOR_ID_1820_AIR_HUW_TEMP, "AirHuwTemp");

  DS1820config.Avg = 0;
  DS1820config.Pin = 7;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_DS1820, SENSOR_ID_1820_OUTDOOR_TEMP,1,&DS1820config,sizeof(DS1820config),50,true);	// 5sec
  tSensorHub::Instance->RegisterLocalSensor(SENSOR_ID_1820_OUTDOOR_TEMP, "OutdoorTemp");

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

  tSensorHub::Instance->subscribeToEvents(SENSOR_ID_1820_HEATING_TEMP,pFloorTemperatureValveControl);
  tSensorHub::Instance->subscribeToEvents(SENSOR_ID_1820_HEATING_TEMP,pRadiatorsTemperatureValveControl);
  tSensorHub::Instance->subscribeToEvents(SENSOR_ID_1820_OUTDOOR_TEMP,&DS1820SensorCallback);



  SimpleDigitalInputSensorConfig.ActiveState = 0;
  SimpleDigitalInputSensorConfig.Pin = A9;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_DIGITAL_INPUT, SENSOR_ID_DIGITAL_WATER_HEATER_REQUEST,1,
		  &SimpleDigitalInputSensorConfig,sizeof(SimpleDigitalInputSensorConfig),10,true);	// 1sec
  tSensorHub::Instance->RegisterLocalSensor(SENSOR_ID_DIGITAL_WATER_HEATER_REQUEST, "WaterHeaterRequest");

  SimpleDigitalInputSensorConfig.ActiveState = 0;
  SimpleDigitalInputSensorConfig.Pin = A8;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_DIGITAL_INPUT, SENSOR_ID_DIGITAL_RADIATORS_PUMP_CONTROL,1,
  	  	  &SimpleDigitalInputSensorConfig,sizeof(SimpleDigitalInputSensorConfig),10,true);	// 1sec
  tSensorHub::Instance->RegisterLocalSensor(SENSOR_ID_DIGITAL_RADIATORS_PUMP_CONTROL, "PumpControl");

  SimpleDigitalInputSensorConfig.ActiveState = 0;
  SimpleDigitalInputSensorConfig.Pin = A7;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_DIGITAL_INPUT, SENSOR_ID_DIGITAL_AUX,1,
  	  	  &SimpleDigitalInputSensorConfig,sizeof(SimpleDigitalInputSensorConfig),10,true);	// 1sec
  tSensorHub::Instance->RegisterLocalSensor(SENSOR_ID_DIGITAL_AUX, "DigitalInputAux");


  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_IMPULSE, SENSOR_ID_IMPULSE_HEATPUMP,1,NULL,0,50,false);
  tSensorHub::Instance->RegisterLocalSensor(SENSOR_ID_IMPULSE_HEATPUMP, "HeatPumpEnergy");
  pinMode(21, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(21), Interrupt, FALLING);
  pSensor->Start();
  
//  pImpulseSensor1 = new tImpulseSensor;
//  pImpulseSensor1->Register(SENSOR_ID_IMPULSE_WATER_HEATER,"AuxHeatEnergy",NULL,50); //5 sec
//  pinMode(20, INPUT_PULLUP);
//  attachInterrupt(digitalPinToInterrupt(20), Interrupt1, FALLING);


  Pt100AnalogSensorConfig.Pin = A14;
  Pt100AnalogSensorConfig.Correction = 8;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_PT100_ANALOG, SENSOR_ID_PT100_HOTAIR,1,&Pt100AnalogSensorConfig,sizeof(Pt100AnalogSensorConfig),50,true);
  tSensorHub::Instance->RegisterLocalSensor(SENSOR_ID_PT100_HOTAIR, "HotAir");


  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_OUTPUT_STATES, SENSOR_ID_OUTPUT_STATES,1,NULL,0,50,true);
  tSensorHub::Instance->RegisterLocalSensor(SENSOR_ID_OUTPUT_STATES, "OutStates");

  
  tHeatingCircleStatusSensor *pHeatingCircleStatusSensor = SensorFactory.CreateSensor(SENSOR_TYPE_HEATING_CIRCLE_STATE, SENSOR_ID_RADIATORS_HEATING_STATE);
  pHeatingCircleStatusSensor->Config.pHeatingControl = pRadiatorsTemperatureValveControl;
  pHeatingCircleStatusSensor->setConfig(50); // 5 sec
  tSensorHub::Instance->RegisterLocalSensor(SENSOR_ID_RADIATORS_HEATING_STATE, "RadiatorsState");
  pHeatingCircleStatusSensor->Start();
  
  pHeatingCircleStatusSensor = SensorFactory.CreateSensor(SENSOR_TYPE_HEATING_CIRCLE_STATE, SENSOR_ID_FLOOR_HEATING_STATE);
  pHeatingCircleStatusSensor->Config.pHeatingControl =  pFloorTemperatureValveControl;
  pHeatingCircleStatusSensor->setConfig(50); // 5 sec
  tSensorHub::Instance->RegisterLocalSensor(SENSOR_ID_FLOOR_HEATING_STATE, "FloorState");
  pHeatingCircleStatusSensor->Start();
#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.print(F("Free RAM: "));
  DEBUG_SERIAL.println(getFreeRam());
  DEBUG_SERIAL.println(F("SYSTEM INITIALIZED"));
#endif
}

void loop() {
  sched.run();
}
