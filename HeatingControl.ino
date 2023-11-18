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
#include "src/Common_code/WatchdogProcess.h"

#include "src/Common_code/sensors/tSensorFactory.h"
#include "src/Common_code/sensors/tSensor.h"
#include "src/Common_code/sensors/tSimpleDigitalInputSensor.h"
#include "src/Common_code/sensors/tDS1820Sensor.h"
#include "src/Common_code/sensors/tImpulseSensor.h"
#include "src/Common_code/sensors/tPt100AnalogSensor.h"
#include "src/Common_code/sensors/tSensorHub.h"
#include "src/Common_code/sensors/tSensorFactory.h"
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
   if (pRequestBuffer->startsWith("/sensorState")) return new tSensorStateServlet();

   return NULL;
}
#endif

#endif // CONFIG_NETWORK

tSensorHub SensorHub;
tSensorFactory SensorFactory;

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
//  tSensor *pSensor;
//  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_SYSTEM_STATUS, SENSOR_ID_SYSTEM_STATUS,1,NULL,0,50,true);
//  pSensor->Register("SystemStatus");
//  pSensor->setSensorSerialEventsMask(EV_TYPE_MEASUREMENT_ERROR | EV_TYPE_MEASUREMENT_COMPLETED);
  
#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.print(F("Free RAM: "));
  DEBUG_SERIAL.println(getFreeRam());
  DEBUG_SERIAL.println(F("SYSTEM INITIALIZED"));
#endif
}

void loop() {
  sched.run();
}
