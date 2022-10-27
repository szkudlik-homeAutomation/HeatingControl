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
#include "src/Common_code/sensors/tSensorStateServlet.h"
#include "src/Common_code/sensors/tSensorHub.h"
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
            DEBUG_SERIAL.print("Measurement completed. devs: ");
            DEBUG_SERIAL.print(pDS1820Result->NumOfDevices);
            DEBUG_SERIAL.print(" Avg: ");
            DEBUG_SERIAL.print(pDS1820Result->Avg);
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
               DEBUG_SERIAL.print(" dev: ");
               DEBUG_SERIAL.print(i);
               DEBUG_SERIAL.print(" temp: ");
               DEBUG_SERIAL.print(((float)(pDS1820Result)->Temp[i]) / 10);
            }
            DEBUG_SERIAL.println();
            break;
   
         case EV_TYPE_MEASUREMENT_ERROR:
            DEBUG_SERIAL.print("Measurement completed. ERROR");
            DEBUG_SERIAL.println();
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
         DEBUG_SERIAL.print("Impulse count: "); 
         DEBUG_SERIAL.print(pResult->Count);
         DEBUG_SERIAL.print(" Sum: "); 
         DEBUG_SERIAL.println(pResult->Sum);
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
         DEBUG_SERIAL.print("PT100 Temp: "); 
         DEBUG_SERIAL.println(pResult->Temperature);
      }
   }
};

tPt100SensorCallback Pt100SensorCallback;
tDS1820SensorCallback DS1820SensorCallback;
tImpulseSensorCallback ImpulseSensorCallback;

tHeatingCircleControl FloorTemperatureValveControl(1,OUT_ID_FLOOR_TEMP_HIGHER,OUT_ID_FLOOR_TEMP_LOWER,OUT_ID_FLOOR_PUMP,2); 
tHeatingCircleControl RadiatorsTemperatureValveControl(0,OUT_ID_RADIATOR_TEMP_HIGHER,OUT_ID_RADIATOR_TEMP_LOWER,OUT_ID_READIATORS_PUMP,2); 

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
  

  tDS1820Sensor::tConfig Ds1820Config;
  Ds1820Config.Pin = 2;
  Ds1820Config.NumOfDevices = 2;
  Ds1820Config.Avg = 0;
#define SENSOR_ID_1820 1
  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_DS1820, SENSOR_ID_1820, "HeatingTemp", &Ds1820Config, 50); //5 sec
  
  FloorTemperatureValveControl.setTargetTemp(28);
  FloorTemperatureValveControl.setTolerance(0.5);
  FloorTemperatureValveControl.setHisteresis(0.7);
  FloorTemperatureValveControl.setFastThold(1); 

  RadiatorsTemperatureValveControl.setTargetTemp(31);
  RadiatorsTemperatureValveControl.setTolerance(0.7);
  RadiatorsTemperatureValveControl.setHisteresis(1);
  RadiatorsTemperatureValveControl.setFastThold(2); 

  SensorHub.subscribeToEvents(SENSOR_ID_1820,&FloorTemperatureValveControl);
  SensorHub.subscribeToEvents(SENSOR_ID_1820,&RadiatorsTemperatureValveControl);
  SensorHub.subscribeToEvents(SENSOR_ID_1820,&DS1820SensorCallback);

  FloorTemperatureValveControl.Start();
  RadiatorsTemperatureValveControl.Start();
//  FloorTemperatureValveControl.Stop();
//  RadiatorsTemperatureValveControl.Stop();


  
#define SENSOR_ID_IMPULSE 2
#define SENSOR_ID_IMPULSE1 3
  
  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_IMPULSE, SENSOR_ID_IMPULSE, "HeatPumpPower", NULL, 50); //5 sec
  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_IMPULSE, SENSOR_ID_IMPULSE1, "AuxHeatPower", NULL, 50); //5 sec

  // local pointers to sensors - interrupts
  pImpulseSensor = tSensor::getSensor(SENSOR_ID_IMPULSE);
  pImpulseSensor1 = tSensor::getSensor(SENSOR_ID_IMPULSE1);
  pinMode(20, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(20), Interrupt, FALLING);
  pinMode(21, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(21), Interrupt1, FALLING);
  
  
  SensorHub.subscribeToEvents(SENSOR_ID_IMPULSE,&ImpulseSensorCallback);
  SensorHub.subscribeToEvents(SENSOR_ID_IMPULSE1,&ImpulseSensorCallback);
  
#define SENSOR_ID_PT100_ANALOG 4
#define SENSOR_ID_PT100_ANALOG1 5

  tPt100AnalogSensor::tConfig Pt100AnalogSensorConfig;
  Pt100AnalogSensorConfig.Pin = A14;
  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_PT100_ANALOG, SENSOR_ID_PT100_ANALOG, "RoomHeatingAir", &Pt100AnalogSensorConfig, 20); //2 sec
  Pt100AnalogSensorConfig.Pin = A15;
  SensorHub.CreateSensorRequest(1, SENSOR_TYPE_PT100_ANALOG, SENSOR_ID_PT100_ANALOG1, "AirPump", &Pt100AnalogSensorConfig, 20); //2 sec
  
  SensorHub.subscribeToEvents(SENSOR_ID_PT100_ANALOG,&Pt100SensorCallback);
  SensorHub.subscribeToEvents(SENSOR_ID_PT100_ANALOG1,&Pt100SensorCallback);

#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.print(F("Free RAM: "));
  DEBUG_SERIAL.println(getFreeRam());
  DEBUG_SERIAL.println(F("SYSTEM INITIALIZED"));
#endif
}


void loop() {
  sched.run();
}
