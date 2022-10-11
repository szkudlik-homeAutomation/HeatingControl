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


class tDS1820SensorCallback : public tSensorEvent
{
public:
   tDS1820SensorCallback() {};
   
   virtual void onEvent(tSensor *pSensor, tEventType EventType)
   {
      tDS1820Sensor::tDS1820Result *pDS1820Result =(tDS1820Sensor::tDS1820Result *) pSensor->getMeasurementBlob();
      uint8_t NumOfItems = pSensor->getMeasurementBlobSize() / sizeof(tDS1820Sensor::tDS1820Result); 
      
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

tDS1820SensorCallback DS1820SensorCallback;

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
  
  pSensor->SetEventCalback(&DS1820SensorCallback);
  pSensor->SetMeasurementPeriod(10);
  pSensor->SetSpecificConfig(&Config);
}


void loop() {
  sched.run();
}
