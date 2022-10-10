#include <ProcessScheduler.h>
#include <Arduino.h>

#include "global.h"

#include "src/lib/TCP_Communication_lib/network.h"
#include "src/lib/TCP_Communication_lib/TcpServer.h"
#include "src/lib/TCP_Communication_lib/HttpServer.h"
#include "src/Common_code/TelnetServer.h"
#include "src/Common_code/WatchdogProcess.h"
#include "src/Common_code/sensors/tSensor.h"
#include "src/Common_code/sensors/tDS1820Sensor.h"
#include "src/tOutputProcessheatingControl.h"
#include "src/servlets.h"
#include "src/tOutputProcessheatingControl.h"


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

   return NULL;
}

int16_t Target = 300;   // 30 degr

// valve servo at full speed 
int16_t Delta1 = 15;   // +- 1.5 degr

// valve at half speed till delta 2

// no correction
int16_t Delta2 = 10;   // +- 1.0 degr


void DS1820SensorCallback(tSensor *pSensor, tSensor::tEventType EventType)
{
   tDS1820Sensor::tDS1820Result *pDS1820Result =(tDS1820Sensor::tDS1820Result *) pSensor->getMeasurementBlob();
   uint8_t NumOfItems = pSensor->getMeasurementBlobSize() / sizeof(tDS1820Sensor::tDS1820Result); 
   
   switch (EventType)
   {
      case tSensor::EV_TYPE_MEASUREMENT_COMPLETED: 
//         DEBUG_SERIAL.print("Measurement completed. devs: ");
//         DEBUG_SERIAL.print(NumOfItems);
//         for (int i = 0; i < NumOfItems; i++)
//         {
//            DEBUG_SERIAL.print(" dev: ");
//            DEBUG_SERIAL.print(i);
//            DEBUG_SERIAL.print(" temp: ");
//            DEBUG_SERIAL.print(((float)(pDS1820Result+i)->Temp) / 10);
//         }
//         DEBUG_SERIAL.println();
         break;

      case tSensor::EV_TYPE_MEASUREMENT_ERROR:
         DEBUG_SERIAL.print("Measurement completed. ERROR");
         DEBUG_SERIAL.println();
         return;
         break;
   }
   
   int16_t Temp = (pDS1820Result+2)->Temp;
   
   uint16_t Delta = abs(Temp - Target);
   uint8_t Output = 255;

   DEBUG_SERIAL.print("-->Temp: ");
   DEBUG_SERIAL.print((float)(Temp) / 10);
   DEBUG_SERIAL.print(" Target: ");
   DEBUG_SERIAL.print((float)(Target) / 10);
   DEBUG_SERIAL.print(" Delta1: ");
   DEBUG_SERIAL.print((float)(Delta1) / 10);
   DEBUG_SERIAL.print(" Delta2: ");
   DEBUG_SERIAL.println((float)(Delta2) / 10);
   
   if (Temp > Target)
   {
      Output = OUT_ID_FLOOR_HEAT_VALVE_CLOSE;
      OutputProcess.SetOutput(OUT_ID_FLOOR_HEAT_VALVE_OPEN,0);
      DEBUG_SERIAL.print("====> DOWN ");
   }
   else
   {
      Output = OUT_ID_FLOOR_HEAT_VALVE_OPEN;
      OutputProcess.SetOutput(OUT_ID_FLOOR_HEAT_VALVE_CLOSE,0);
      DEBUG_SERIAL.print("====>   UP ");
   }
   
   if (Delta > Delta1)
   {
      OutputProcess.SetOutput(Output,1);
      DEBUG_SERIAL.println("FAST");
   }
   else if (Delta > Delta2)
   {
      OutputProcess.SetOutput(Output,1,2);
      DEBUG_SERIAL.println("SLOW");
   }
   else
   {
      OutputProcess.SetOutput(Output,0);
      DEBUG_SERIAL.println("STOP");
   }
}

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

#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.println("SYSTEM INITIALIZED");
#endif
  

  tSensor::Create(SENSOR_TYPE_DS1820,1);
  tDS1820Sensor::tDS1820SensorConfig Config;
  Config.Pin = 2;
  Config.NumOfDevices = 3;
  Config.Avg = 0;
  
  tSensor *pSensor = tSensor::getSensor(1);
  
  pSensor->SetCalback(DS1820SensorCallback);
  pSensor->SetMeasurementPeriod(30);   //3 sec
  pSensor->SetSpecificConfig(&Config);
}


void loop() {
  sched.run();
}
