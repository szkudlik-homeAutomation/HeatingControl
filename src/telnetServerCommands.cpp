/*
 * telnetServerCommands.cpp
 *
 *  Created on: 6 pa� 2022
 *      Author: szkud
 */

#include "../global.h"
#if CONFIG_TELNET_SERVER

#include "lib/Commander/src/Commander.h"
#include "Common_code/Network/telnetServer.h"
#include "tHeatingControlOutgoingFrames.h"

const commandList_t TelnetCommands[] = {
  {"enableLogs",      TelnetEnableLogs,             "enable logs on telnet console"},
  {"disableLogs",     TelnetDisableLogs,            "enable logs on telnet console"},
#if CONFIG_TLE8457_COMM_LIB
  {"GetVersion",      send_GetVersion,              "show version"},
  {"Reset",           send_Reset,                   "reset the system"},
  {"ScanActiveNodes", trigger_ScanNodes,            "Scan the bus for nodes from 1 to 32"},
  {"StateOverview",   send_stateOverviewHandler,    "StateOverview dev_id"},
  {"OutputState",     send_OutputStateHandler,      "OutputState dev_id out_id"},
  {"SetOutput",       send_SetOutputHandler,        "SetOutput dev_id out_id state [timer] (not set=default, 0-forever)"},
#if CONFIG_TELNET_COMMANDS_SENSORS
  {"GetSensorById",   send_GetSensorByIdReqestHandler,"GetSensorById sensor_id [dst_dev = broadcast]"},
  {"GetSensorMeasurement",  send_GetSensorMeasurementReqest, "GetSensorMeasurement sensor_id [dst_dev = broadcast]"},
  {"CreateSensor", send_CreateSensorRequest, "CreateSensor dev_id sensor_type sensor_id"},
  {"StartSensor", send_StartSensorRequest, "StartSensor sensor_id [sensor_ev_mask = EV_TYPE_MEASUREMENT_COMPLETED] [dev_id = broadcast]"},
  {"StopSensor", send_StopSensorRequest, "StopSensor sensor_id [dev_id = broadcast]"},
  {"ConfigureSensor", send_ConfigureSensorRequest, "ConfigureSensor sensor_id period [dev_id = broadcast]"},
  {"SaveSensorsToEeprom", send_saveSensorsToEeprom, "SaveSensorsToEeprom dst_dev"},
  {"RestoreSensorsFromEeprom", send_restoreSensorsFromEeprom, "RestoreSensorsFromEeprom dst_dev"},

#endif // CONFIG_TELNET_COMMANDS_SENSORS
#endif //CONFIG_TLE8457_COMM_LIB
};

tTelnetServer TelnetServer(TelnetCommands,sizeof(TelnetCommands));


#endif CONFIG_TELNET_SERVER
