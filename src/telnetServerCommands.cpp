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
#include "OutgoingMessage.h"

bool send_GetVersion(Commander &Cmdr);
bool send_Reset(Commander &Cmdr);


const commandList_t TelnetCommands[] = {
  {"enableLogs",      TelnetEnableLogs,             "enable logs on telnet console"},
  {"disableLogs",     TelnetDisableLogs,            "enable logs on telnet console"},
  {"GetVersion",      send_GetVersion,              "show version"},
  {"Reset",           send_Reset,                   "reset the system"},
};

tTelnetServer TelnetServer(TelnetCommands,sizeof(TelnetCommands));

bool send_GetVersion(Commander &Cmdr)
{
  int Dst;
  if(Cmdr.getInt(Dst))
  {
	  OutgoingMessage::SendMsgVersionRequest(Dst);
  }
  else
  {
    Cmdr.println(F("Usage: GetVersion dst_dev_id"));
    return false;
  }

  return true;
}


bool send_Reset(Commander &Cmdr)
{
//  int Dst;
//  if(Cmdr.getInt(Dst))
//  {
//    Worker.SendMsgReset(Dst);
//  }
//  else
//  {
//    Cmdr.println(F("Usage: SendReset dst_dev_id"));
//    return false;
//  }
//
//  return true;
}


bool trigger_ScanNodes(Commander &Cmdr)
{
//  Worker.triggerNodesScan();
}

#endif CONFIG_TELNET_SERVER
