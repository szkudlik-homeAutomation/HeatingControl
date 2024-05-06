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
   if (pRequestBuffer->startsWith("/heat")) return new tHeatingControlServletTMP();
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


#include "SensorCallbacksPOC.h"

tPt100SensorCallback Pt100SensorCallback(SENSOR_ID_PT100_HOTAIR);
tDS1820SensorCallback DS1820SensorCallback;	// all DS1820
tImpulseSensorCallback ImpulseSensorCallback(SENSOR_ID_IMPULSE_HEATPUMP);

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



  tSensor *pSensor;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_SYSTEM_STATUS, SENSOR_ID_SYSTEM_STATUS,1,NULL,0,50,true);
  tSensorHub::Instance->RegisterSensor(SENSOR_ID_SYSTEM_STATUS, "SystemStatus");

  tDS1820Sensor::tConfig DS1820config;
  tSimpleDigitalInputSensor::tConfig SimpleDigitalInputSensorConfig;
  tPt100AnalogSensor::tConfig Pt100AnalogSensorConfig;
  
  DS1820config.Avg = 0;
  DS1820config.Pin = 2;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_DS1820, SENSOR_ID_1820_HEATING_TEMP,1,&DS1820config,sizeof(DS1820config),50,true);	// 5sec
  tSensorHub::Instance->RegisterSensor(SENSOR_ID_1820_HEATING_TEMP, "HeatingTemp");
  
  DS1820config.Avg = 0;
  DS1820config.Pin = 2;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_DS1820, SENSOR_ID_1820_AIR_HUW_TEMP,1,&DS1820config,sizeof(DS1820config),50,true);	// 5sec
  tSensorHub::Instance->RegisterSensor(SENSOR_ID_1820_AIR_HUW_TEMP, "AirHuwTemp");

  DS1820config.Avg = 0;
  DS1820config.Pin = 7;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_DS1820, SENSOR_ID_1820_OUTDOOR_TEMP,1,&DS1820config,sizeof(DS1820config),50,true);	// 5sec
  tSensorHub::Instance->RegisterSensor(SENSOR_ID_1820_OUTDOOR_TEMP, "OutdoorTemp");

  pFloorTemperatureValveControl = new tHeatingCircleControl(
		SENSOR_ID_1820_HEATING_TEMP,
        FloorTemperatureTempSensorSerial,
        HeatingSourceTempSensorSerial,
        HeatingStorageTempSensorSerial,
        OUT_ID_FLOOR_TEMP_HIGHER,
        OUT_ID_FLOOR_TEMP_LOWER,
        OUT_ID_FLOOR_PUMP,
        2); 
  pRadiatorsTemperatureValveControl = new tHeatingCircleControl(
	    SENSOR_ID_1820_HEATING_TEMP,
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

  SimpleDigitalInputSensorConfig.ActiveState = 0;
  SimpleDigitalInputSensorConfig.Pin = A9;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_DIGITAL_INPUT, SENSOR_ID_DIGITAL_WATER_HEATER_REQUEST,1,
		  &SimpleDigitalInputSensorConfig,sizeof(SimpleDigitalInputSensorConfig),10,true);	// 1sec
  tSensorHub::Instance->RegisterSensor(SENSOR_ID_DIGITAL_WATER_HEATER_REQUEST, "WaterHeaterRequest");

  SimpleDigitalInputSensorConfig.ActiveState = 0;
  SimpleDigitalInputSensorConfig.Pin = A8;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_DIGITAL_INPUT, SENSOR_ID_DIGITAL_RADIATORS_PUMP_CONTROL,1,
  	  	  &SimpleDigitalInputSensorConfig,sizeof(SimpleDigitalInputSensorConfig),10,true);	// 1sec
  tSensorHub::Instance->RegisterSensor(SENSOR_ID_DIGITAL_RADIATORS_PUMP_CONTROL, "PumpControl");

  SimpleDigitalInputSensorConfig.ActiveState = 0;
  SimpleDigitalInputSensorConfig.Pin = A7;
  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_DIGITAL_INPUT, SENSOR_ID_DIGITAL_AUX,1,
  	  	  &SimpleDigitalInputSensorConfig,sizeof(SimpleDigitalInputSensorConfig),10,true);	// 1sec
  tSensorHub::Instance->RegisterSensor(SENSOR_ID_DIGITAL_AUX, "DigitalInputAux");


  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_IMPULSE, SENSOR_ID_IMPULSE_HEATPUMP,1,NULL,0,50,false);
  tSensorHub::Instance->RegisterSensor(SENSOR_ID_IMPULSE_HEATPUMP, "HeatPumpEnergy");
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
  tSensorHub::Instance->RegisterSensor(SENSOR_ID_PT100_HOTAIR, "HotAir");


  pSensor = SensorFactory.CreateSensor(SENSOR_TYPE_OUTPUT_STATES, SENSOR_ID_OUTPUT_STATES,1,NULL,0,50,true);
  tSensorHub::Instance->RegisterSensor(SENSOR_ID_OUTPUT_STATES, "OutStates");

  
  tHeatingCircleStatusSensor *pHeatingCircleStatusSensor = SensorFactory.CreateSensor(SENSOR_TYPE_HEATING_CIRCLE_STATE, SENSOR_ID_RADIATORS_HEATING_STATE);
  pHeatingCircleStatusSensor->Config.pHeatingControl = pRadiatorsTemperatureValveControl;
  pHeatingCircleStatusSensor->setConfig(50); // 5 sec
  tSensorHub::Instance->RegisterSensor(SENSOR_ID_RADIATORS_HEATING_STATE, "RadiatorsState");
  pHeatingCircleStatusSensor->Start();
  
  pHeatingCircleStatusSensor = SensorFactory.CreateSensor(SENSOR_TYPE_HEATING_CIRCLE_STATE, SENSOR_ID_FLOOR_HEATING_STATE);
  pHeatingCircleStatusSensor->Config.pHeatingControl =  pFloorTemperatureValveControl;
  pHeatingCircleStatusSensor->setConfig(50); // 5 sec
  tSensorHub::Instance->RegisterSensor(SENSOR_ID_FLOOR_HEATING_STATE, "FloorState");
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
