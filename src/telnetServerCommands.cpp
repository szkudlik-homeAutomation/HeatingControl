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
  {"StateOverview",   send_stateOverviewHandler,    "MESSAGE_TYPE_REQUEST_OVERVIEW_STATE dev_id"},
  {"OutputState",     send_OutputStateHandler,      "MESSAGE_TYPE_OUTPUT_STATE_REQUEST dev_id out_id"},
  {"SetOutput",       send_SetOutputHandler,        "MESSAGE_TYPE_OUTPUT_STATE_REQUEST dev_id out_id state [timer] (not set=default, 0-forever)"},
#if CONFIG_SENSORS
  {"GetSensorById",   send_GetSensorByIdReqestHandler,"MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST sensor_id [dst_dev]   if dst is not provided => broadcast"},
  {"GetSensorMeasurement",  send_GetSensorMeasurementReqest, "MESSAGE_TYPE_SENSOR_MEASUREMENT_REQUEST sensor_id [dst_dev]   if dst is not provided => broadcast"},
  {"CreateSensor", send_CreateSensorRequest, "MESSAGE_TYPE_SENSOR_CREATE dev_id sensor_type sensor_id"},
  {"StartSensor", send_StartSensorRequest, "MESSAGE_TYPE_SENSOR_START dev_id sensor_id [sensor_ev_mask = EV_TYPE_MEASUREMENT_COMPLETED]"},
  {"StopSensor", send_StopSensorRequest, "MESSAGE_TYPE_SENSOR_STOP dev_id sensor_id"},
#endif // CONFIG_SENSORS
#endif //CONFIG_TLE8457_COMM_LIB
};

tTelnetServer TelnetServer(TelnetCommands,sizeof(TelnetCommands));


#endif CONFIG_TELNET_SERVER
