#include "global.h"

// FORWARDS OF REQUIRED FUNCTIONS
void AppSetup();

#include "src/lib/ArduinoProcessScheduler/src/ProcessScheduler.h"

#include "src/Common_code/Network/network.h"
#include "src/Common_code/Network/TcpServer.h"
#include "src/Common_code/Network/HttpServer.h"
#include "src/Common_code/Network/TelnetServer.h"
#include "src/Common_code/Network/servlets/servlets.h"
#include "src/Common_code/Network/servlets/tOutputControlServlets.h"
#include "src/Common_code/Network/servlets/tSensorStateServlet.h"

#include "src/Common_code/WatchdogProcess.h"
#include "src/Common_code/WorkerProcess.h"
#include "src/Common_code/sensors/tSensor.h"
#include "src/Common_code/sensors/tRemoteSensorProcess.h"
#include "src/Common_code/sensors/tSensorHub.h"
#include "src/Common_code/sensors/tRemoteSensorHub.h"
#include "src/Common_code/sensors/tOutputStateSensor.h"
#include "src/Common_code/sensors/tSimpleDigitalInputSensor.h"
#include "src/Common_code/sensors/tSystemStatusSensor.h"
#include "src/Common_code/TLE8457_serial/TLE8457_serial_lib.h"
#include "src/Common_code/TLE8457_serial/tIncomingFrameHanlder.h"
#include "src/tHeatingControlSensorFactory.h"

#if APP_HeatingCentral
    #include "src/AppSpecific/HeatingCentral/tOutputProcess_appSpecific.h"
    #include "src/AppSpecific/HeatingCentral/tHeatingControlServlet.h"
#elif APP_generalTest
    #include "src/AppSpecific/GeneralTest/tOutputProcess_appSpecific.h"
#endif

// global objects - all apps
Scheduler sched;

#if CONFIG_WATCHDOG
    tWatchdogProcess WatchdogProcess(sched);
#endif //CONFIG_WATCHDOG

#if CONFIG_WORKER_PROCESS
    WorkerProcess Worker(sched);
#endif

#if CONFIG_SENSORS
    #if CONFIG_SENSOR_HUB
    #if CONFIG_SENSORS_OVER_SERIAL_COMM
        tRemoteSensorHub SensorHub;
    #else //CONFIG_SENSORS_OVER_SERIAL_COMM
        tSensorHub SensorHub;
    #endif //CONFIG_SENSORS_OVER_SERIAL_COMM
    #endif // CONFIG_SENSOR_HUB
    
    #if CONFIG_SENSORS_OVER_SERIAL_COMM
    	tRemoteSensorProcess SensorProcess(sched); // sensors actions and sensor management from remote
    #else
    	tSensorProcess SensorProcess(sched);       // all sensors actions
    #endif //CONFIG_SENSORS_OVER_SERIAL_COMM
    
    tHeatingConrolSensorFactory SensorFactory;
#endif // CONFIG_SENSORS

#if CONFIG_TLE8457_COMM_LIB
    tIncomingFrameHanlder IncomingFrameHandler;
    CommRecieverProcess CommReciever(sched,EEPROM.read(EEPROM_DEVICE_ID_OFFSET));
    CommSenderProcess CommSender(sched,EEPROM.read(EEPROM_DEVICE_ID_OFFSET),EEPROM.read(EEPROM_DEVICE_ID_OFFSET));

    void COMM_SERIAL_EVENT() {
      CommReciever.serialEvent();
    }
#endif //CONFIG_TLE8457_COMM_LIB


#if CONFIG_NETWORK
    tNetwork Network;
    tTcpServerProcess TcpServerProcess(sched, TCP_WATCHDOG_TIMEOUT);
    #if CONFIG_HTTP_SERVER
        tHttpServer HttpServer;
        
        tHttpServlet * ServletFactory(String *pRequestBuffer)
        {
           if (pRequestBuffer->startsWith("/OutputControl.js")) return new tOutputControlJavaScript();
           if (pRequestBuffer->startsWith("/outputState")) return new tOutputStateServlet();
           if (pRequestBuffer->startsWith("/outputSet")) return new tOutputSetServlet();
        #if CONFIG_SENSOR_STATE_SERVLET 
           if (pRequestBuffer->startsWith("/sensorState")) return new tSensorStateServlet();
        #endif // CONFIG_SENSOR_STATE_SERVLET
        #if CONFIG_HEATING_CIRCLE_CONTROL_SERVLET
           if (pRequestBuffer->startsWith("/heat")) return new tHeatingControlServlet();
        #endif // CONFIG_HEATING_CIRCLE_CONTROL_SERVLET
           return NULL;
        }
    #endif

    #if CONFIG_TELNET_SERVER
        extern tTelnetServer TelnetServer;
#endif
#endif // CONFIG_NETWORK

#if CONFIG_OUTPUT_PROCESS
tOutputProcess_appSpecific OutputProcess(sched);
#endif





void setup() {
#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.begin(115200);
  while (!DEBUG_SERIAL);
  DEBUG_SERIAL.print(F("START, v"));
  DEBUG_SERIAL.println(FW_VERSION);
#endif

  if (EEPROM.read(EEPROM_CANNARY_OFFSET) != EEPROM_CANNARY)
    SetDefaultEEPromValues();

#if CONFIG_TLE8457_COMM_LIB
  COMM_SERIAL.begin(CONFIG_TRANSMISSION_SPEED);
  while (!COMM_SERIAL);

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

#if CONFIG_SENSORS
  SensorProcess.add(true);
#endif
  
#if CONFIG_WORKER_PROCESS
  Worker.add();
#endif //CONFIG_WORKER_PROCESS

#if CONFIG_WATCHDOG
  WatchdogProcess.add(true);
#endif

#if CONFIG_OUTPUT_PROCESS
  OutputProcess.add(true);
#endif


    AppSetup();
    
#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.print(F("Free RAM: "));
  DEBUG_SERIAL.println(getFreeRam());
  DEBUG_SERIAL.println(F("SYSTEM INITIALIZED"));
#endif
}

void loop() {
  sched.run();
}
