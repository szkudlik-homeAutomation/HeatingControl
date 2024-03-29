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
#include "src/Common_code/sensors/tRemoteSensorProcess.h"
#include "src/Common_code/sensors/tSimpleDigitalInputSensor.h"
#include "src/Common_code/sensors/tDS1820Sensor.h"
#include "src/Common_code/sensors/tImpulseSensor.h"
#include "src/Common_code/sensors/tPt100AnalogSensor.h"
#include "src/Common_code/sensors/tSensorHub.h"
#include "src/Common_code/sensors/tRemoteSensorHub.h"
#include "src/sensors/tHeatingCircleStatusSensor.h"
#include "src/Common_code/sensors/tOutputStateSensor.h"
#include "src/Common_code/sensors/tSimpleDigitalInputSensor.h"
#include "src/Common_code/sensors/tSystemStatusSensor.h"
#include "src/tOutputProcessheatingControl.h"
#include "src/tOutputProcessheatingControl.h"
#include "src/tHeatingCircleControl.h"
#include "src/Common_code/TLE8457_serial/TLE8457_serial_lib.h"
#include "src/Common_code/TLE8457_serial/tIncomingFrameHanlder.h"
#include "src/Common_code/WorkerProcess.h"
#include "src/tHeatingControlSensorFactory.h"

// restart if no connection for 5 minutes
#define TCP_WATCHDOG_TIMEOUT 300 

Scheduler sched;
#if CONFIG_TLE8457_COMM_LIB
	tRemoteSensorProcess SensorProcess(sched);
#else
	tSensorProcess SensorProcess(sched);
#endif
//WorkerProcess Worker(sched);
tOutputProcess_heatingControl OutputProcess(sched);
tWatchdogProcess WatchdogProcess(sched);

#if CONFIG_TLE8457_COMM_LIB
tIncomingFrameHanlder IncomingFrameHandler;
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
   return NULL;
}
#endif

#endif // CONFIG_NETWORK

#if CONFIG_SENSOR_HUB
#if CONFIG_SENSORS_OVER_SERIAL_COMM
	tRemoteSensorHub SensorHub;
#else //CONFIG_SENSORS_OVER_SERIAL_COMM
	tSensorHub SensorHub;
#endif //CONFIG_SENSORS_OVER_SERIAL_COMM
#endif // CONFIG_SENSOR_HUB

tHeatingConrolSensorFactory SensorFactory;

void setup() {
#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.begin(115200);
  while (!DEBUG_SERIAL);
  DEBUG_SERIAL.print(F("START, v"));
  DEBUG_SERIAL.println(FW_VERSION);
#endif

  if (EEPROM.read(EEPROM_CANNARY_OFFSET) != EEPROM_CANNARY)
    SetDefaultEEPromValues();

  COMM_SERIAL.begin(TRANSMISSION_SPEED);
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
#define REMOTE_SENSOR_ID_SYSTEM_STATUS 2
#define MANUAL_SENSOR_ID 3

  tSensor *pSensor;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_SYSTEM_STATUS, SENSOR_ID_SYSTEM_STATUS,1,NULL,0,50,true);
  tSensorHub::Instance->RegisterSensor(SENSOR_ID_SYSTEM_STATUS, "SystemStatus");
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_SYSTEM_STATUS, REMOTE_SENSOR_ID_SYSTEM_STATUS,1,NULL,0,50,true);
  tSensorHub::Instance->RegisterSensor(REMOTE_SENSOR_ID_SYSTEM_STATUS, "RemoteSystemStatus");

  tSensorHub::Instance->RegisterSensor(MANUAL_SENSOR_ID, "ManualSensor");

#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.print(F("Free RAM: "));
  DEBUG_SERIAL.println(getFreeRam());
  DEBUG_SERIAL.println(F("SYSTEM INITIALIZED"));
#endif
}

void loop() {
  sched.run();
}
