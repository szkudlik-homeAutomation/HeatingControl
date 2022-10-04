#include <ProcessScheduler.h>
#include <Arduino.h>

#include "src/common.h"
#include "src/Eeprom.h"
#include "src/version.h"

#include "src/lib/TCP_Communication_lib/network.h"
#include "src/lib/TCP_Communication_lib/TcpServer.h"
#include "src/lib/TCP_Communication_lib/HttpServer.h"
#include "src/TelnetServer.h"
#include "src/WatchdogProcess.h"

#include "src/lib/Sensors_lib/tDS1820Sensor.h"
#include "src/lib/Sensors_lib/tSensor.h"
#include "src/servlets.h"

Scheduler sched;
tSensorProcess SensorProcess(sched); 
//WorkerProcess Worker(sched);
tWatchdogProcess WatchdogProcess(sched);

tNetwork Network;
tTcpServerProcess TcpServerProcess(sched);
tHttpServer HttpServer;
tTelnetServer TelnetServer;

tHttpServlet * ServletFactory(String *pRequestBuffer)
{
   return new tDS1820Servlet();
}



void DS1820SensorCallback(tSensor *pSensor, tSensor::tEventType EventType)
{
   tDS1820Sensor::tDS1820Result *pDS1820Result =(tDS1820Sensor::tDS1820Result *) pSensor->getMeasurementBlob();
   uint8_t NumOfItems = pSensor->getMeasurementBlobSize() / sizeof(tDS1820Sensor::tDS1820Result); 
   
   switch (EventType)
   {
      case tSensor::EV_TYPE_MEASUREMENT_COMPLETED: 
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
         
      case tSensor::EV_TYPE_MEASUREMENT_ERROR:
         DEBUG_SERIAL.print("Measurement completed. ERROR");
         DEBUG_SERIAL.println();
         break;
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
  WatchdogProcess.add(true);

  tSensor::Create(SENSOR_TYPE_DS1820,1);

  tDS1820Sensor *pDS1820Sensor = tSensor::getSensor(1);
  
  tDS1820Sensor::tDS1820SensorConfig Config;
  Config.Pin = 2;
  Config.Avg = 0;
  pDS1820Sensor->SetSpecificConfig(&Config);
  tDS1820Servlet::registerSensor(pDS1820Sensor);
  pDS1820Sensor->SetCalback(DS1820SensorCallback);

  pDS1820Sensor->SetMeasurementPeriod(1000/SENSOR_PROCESS_SERVICE_TIME);
  
#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.println("SYSTEM INITIALIZED");
#endif
}


void loop() {
  sched.run();
}
