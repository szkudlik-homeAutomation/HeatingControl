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
#endif //CONFIG_TLE8457_COMM_LIB
};

tTelnetServer TelnetServer(TelnetCommands,sizeof(TelnetCommands));


#endif CONFIG_TELNET_SERVER
