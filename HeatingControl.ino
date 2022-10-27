#include "src/lib/external/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include <Arduino.h>

#include "global.h"

#include "src/lib/TCP_Communication_lib/network.h"
#include "src/lib/TCP_Communication_lib/TcpServer.h"
#include "src/lib/TCP_Communication_lib/HttpServer.h"
#include "src/Common_code/TelnetServer.h"
#include "src/Common_code/WatchdogProcess.h"
#include "src/Common_code/sensors/tSensor.h"
#include "src/Common_code/sensors/tDS1820Sensor.h"
#include "src/Common_code/sensors/tImpulseSensor.h"
#include "src/Common_code/sensors/tPt100AnalogSensor.h"
#include "src/tOutputProcessheatingControl.h"
#include "src/servlets.h"
#include "src/tOutputProcessheatingControl.h"
#include "src/tHeatingCircleControl.h"

Scheduler sched;
tSensorProcess SensorProcess(sched); 
//WorkerProcess Worker(sched);
tOutputProcess_heatingControl OutputProcess(sched);
tWatchdogProcess WatchdogProcess(sched);

tNetwork Network;
tTcpServerProcess TcpServerProcess(sched);
tHttpServer HttpServer;
extern tTelnetServer TelnetServer;

tHttpServlet * ServletFactory(String *pRequestBuffer)
{
   if (pRequestBuffer->startsWith("/1.js")) return new tjavaScriptServlet();
   if (pRequestBuffer->startsWith("/outputState")) return new tOutputStateServlet();
   if (pRequestBuffer->startsWith("/outputSet")) return new tOutputSetServlet();
   if (pRequestBuffer->startsWith("/heat")) return new tHeatingControlServletTMP();

   return NULL;
}

class tDS1820SensorCallback : public tSensorEvent
{
public:
   tDS1820SensorCallback() {};
   
   virtual void onEvent(tSensor *pSensor, tEventType EventType)
   {
      tDS1820Sensor::tResult *pDS1820Result =(tDS1820Sensor::tResult *) pSensor->getMeasurementBlob();
      uint8_t NumOfItems = pSensor->getMeasurementBlobSize() / sizeof(tDS1820Sensor::tResult); 
      
      switch (EventType)
      {
         case tSensorEvent::EV_TYPE_MEASUREMENT_COMPLETED: 
            DEBUG_SERIAL.print("Measurement completed. devs: ");
            DEBUG_SERIAL.print(NumOfItems);
            for (int i = 0; i < NumOfItems; i++)
            {
               DEBUG_SERIAL.print(" dev: ");
               DEBUG_SERIAL.print(i);
               DEBUG_SERIAL.print(" temp: ");
               DEBUG_SERIAL.print(((float)(pDS1820Result+i)->Temp) / 10);
            }
            DEBUG_SERIAL.println();
            break;
   
         case tSensorEvent::EV_TYPE_MEASUREMENT_ERROR:
            DEBUG_SERIAL.print("Measurement completed. ERROR");
            DEBUG_SERIAL.println();
            break;
      }
   }
};

class tImpulseSensorCallback : public tSensorEvent
{
public:
   tImpulseSensorCallback() {};
   virtual void onEvent(tSensor *pSensor, tEventType EventType)
   {
      if (tSensorEvent::EV_TYPE_MEASUREMENT_COMPLETED == EventType)
      {
         tImpulseSensor::tResult *pResult =(tImpulseSensor::tResult *) pSensor->getMeasurementBlob();
         DEBUG_SERIAL.print("Impulse count: "); 
         DEBUG_SERIAL.print(pResult->Count);
         DEBUG_SERIAL.print(" Sum: "); 
         DEBUG_SERIAL.println(pResult->Sum);
      }
   }
};

class tPt100SensorCallback : public tSensorEvent
{
public:
   tPt100SensorCallback() {};
   virtual void onEvent(tSensor *pSensor, tEventType EventType)
   {
      if (tSensorEvent::EV_TYPE_MEASUREMENT_COMPLETED == EventType)
      {
         tPt100AnalogSensor::tResult *pResult =(tPt100AnalogSensor::tResult *) pSensor->getMeasurementBlob();
         DEBUG_SERIAL.print("Temp: "); 
         DEBUG_SERIAL.println(pResult->Temperature);
      }
   }
};

tPt100SensorCallback Pt100SensorCallback;
tDS1820SensorCallback DS1820SensorCallback;

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

tImpulseSensorCallback ImpulseSensorCallback;

tHeatingCircleControl FloorTemperatureValveControl(1,OUT_ID_FLOOR_TEMP_HIGHER,OUT_ID_FLOOR_TEMP_LOWER,OUT_ID_FLOOR_PUMP,2); 
tHeatingCircleControl RadiatorsTemperatureValveControl(0,OUT_ID_RADIATOR_TEMP_HIGHER,OUT_ID_RADIATOR_TEMP_LOWER,OUT_ID_READIATORS_PUMP,2); 

void setup() {
  if (EEPROM.read(EEPROM_CANNARY_OFFSET) != EEPROM_CANNARY)
    SetDefaultEEPromValues();

#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.begin(115200);
  while (!DEBUG_SERIAL);
  DEBUG_SERIAL.print("START, v");
  DEBUG_SERIAL.println(FW_VERSION);
#endif

  Network.init();
  TcpServerProcess.add(true);
#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.println("START Tcp ");
#endif

  SensorProcess.add(true);
//  Worker.add();
  OutputProcess.add(true);
  WatchdogProcess.add(true);
  

  tSensor::Create(SENSOR_TYPE_DS1820,1);
  tDS1820Sensor::tConfig Config;
  Config.Pin = 2;
  Config.NumOfDevices = 2;
  Config.Avg = 0;
  
  tSensor *pSensor = tSensor::getSensor(1);
  
  pSensor->SetMeasurementPeriod(50);   //5 sec
  pSensor->SetSpecificConfig(&Config);

  FloorTemperatureValveControl.setTargetTemp(28);
  FloorTemperatureValveControl.setTolerance(0.5);
  FloorTemperatureValveControl.setHisteresis(0.7);
  FloorTemperatureValveControl.setFastThold(1); 

  RadiatorsTemperatureValveControl.setTargetTemp(31);
  RadiatorsTemperatureValveControl.setTolerance(0.7);
  RadiatorsTemperatureValveControl.setHisteresis(1);
  RadiatorsTemperatureValveControl.setFastThold(2); 

  
  pSensor->SetEventCalback(&FloorTemperatureValveControl);
  pSensor->SetEventCalback(&RadiatorsTemperatureValveControl);
  
  pSensor->SetEventCalback(&DS1820SensorCallback);
  FloorTemperatureValveControl.Start();
  RadiatorsTemperatureValveControl.Start();
//  FloorTemperatureValveControl.Stop();
//  RadiatorsTemperatureValveControl.Stop();


   tSensor::Create(SENSOR_TYPE_IMPULSE,2);
   pImpulseSensor = tSensor::getSensor(2);

   tSensor::Create(SENSOR_TYPE_IMPULSE,3);
   pImpulseSensor1 = tSensor::getSensor(3);

   pImpulseSensor->SetMeasurementPeriod(20);   //2 sec
   pImpulseSensor->SetEventCalback(&ImpulseSensorCallback);

   pImpulseSensor1->SetMeasurementPeriod(20);   //2 sec
   pImpulseSensor1->SetEventCalback(&ImpulseSensorCallback);

   pinMode(20, INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(20), Interrupt, FALLING);
   pinMode(21, INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(21), Interrupt1, FALLING);
   
   tSensor::Create(SENSOR_TYPE_PT100_ANALOG,4);
   tSensor::Create(SENSOR_TYPE_PT100_ANALOG,5);

   pSensor = tSensor::getSensor(4);
   tPt100AnalogSensor::tConfig Pt100AnalogSensorConfig;
   Pt100AnalogSensorConfig.Pin = A14;
   pSensor->SetMeasurementPeriod(20);   //2 sec
   pSensor->SetSpecificConfig(&Pt100AnalogSensorConfig);
   pSensor->SetEventCalback(&Pt100SensorCallback);

   pSensor = tSensor::getSensor(5);
   Pt100AnalogSensorConfig.Pin = A15;
   pSensor->SetMeasurementPeriod(20);   //2 sec
   pSensor->SetSpecificConfig(&Pt100AnalogSensorConfig);
   pSensor->SetEventCalback(&Pt100SensorCallback);

#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.print("Free RAM: ");
  DEBUG_SERIAL.println(getFreeRam());
  DEBUG_SERIAL.println("SYSTEM INITIALIZED");
#endif
}


void loop() {
  sched.run();
}
