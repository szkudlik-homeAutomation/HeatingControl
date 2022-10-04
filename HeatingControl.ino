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
#include "src/OutputProcess.h"

Scheduler sched;
//tSensorProcess SensorProcess(sched); 
//WorkerProcess Worker(sched);
tOutputProcess OutputProcess(sched);
tWatchdogProcess WatchdogProcess(sched);

tNetwork Network;
tTcpServerProcess TcpServerProcess(sched);
tHttpServer HttpServer;
tTelnetServer TelnetServer;

tHttpServlet * ServletFactory(String *pRequestBuffer)
{
   return NULL;
}

//tSensor *SensorFactory(uint8_t SensorType)
//{
//   return NULL;
//}
//

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

//  SensorProcess.add(true);
//  Worker.add();
  OutputProcess.add(true);
  WatchdogProcess.add(true);

#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.println("SYSTEM INITIALIZED");
#endif
  

}


void loop() {
  sched.run();
}
