/*
 * telnetServerCommands.cpp
 *
 *  Created on: 6 pa� 2022
 *      Author: szkud
 */

#include "../global.h"
#include <Commander.h>
#include "Common_code/telnetServer.h"
#include "tHeatingCircleControl.h"

bool enableLogs(Commander &Cmdr);
bool disableLogs(Commander &Cmdr);
bool send_GetVersion(Commander &Cmdr);
bool send_Reset(Commander &Cmdr);
bool floor_tmp(Commander &Cmdr);
bool rad_tmp(Commander &Cmdr);


const commandList_t TelnetCommands[] = {
  {"enableLogs",      enableLogs,                   "enable logs on telnet console"},
  {"disableLogs",     disableLogs,                  "enable logs on telnet console"},
  {"GetVersion",      send_GetVersion,              "show version"},
  {"Reset",           send_Reset,                   "reset the system"},
  {"floor",           floor_tmp,                    "floor 0/1 temp"},
  {"rad",           rad_tmp,                    "rad 0/1 temp"},
};

tTelnetServer TelnetServer(TelnetCommands,sizeof(TelnetCommands));


extern tHeatingCircleControl FloorTemperatureValveControl;
extern tHeatingCircleControl RadiatorsTemperatureValveControl;



bool floor_tmp(Commander &Cmdr)
{
   int on;
   float temp;
   if(! Cmdr.getInt(on))
      goto error;
   if(! Cmdr.getFloat(temp))
      goto error;

   if (on) FloorTemperatureValveControl.Start();
   else  FloorTemperatureValveControl.Stop();

   FloorTemperatureValveControl.setTargetTemp(temp);

   return true;
error:
   Cmdr.println(F("Usage: floor 1/0 temp"));
   return false;
}

bool rad_tmp(Commander &Cmdr)
{
   int on;
   float temp;
   if(! Cmdr.getInt(on))
      goto error;
   if(! Cmdr.getFloat(temp))
      goto error;

   if (on) RadiatorsTemperatureValveControl.Start();
   else  RadiatorsTemperatureValveControl.Stop();

   RadiatorsTemperatureValveControl.setTargetTemp(temp);

   return true;
error:
   Cmdr.println(F("Usage: rad 1/0 temp"));
   return false;
}


bool enableLogs(Commander &Cmdr)
{
   TelnetServer.EnableLogs();
}

bool disableLogs(Commander &Cmdr)
{
   TelnetServer.DisableLogs();
}

bool send_GetVersion(Commander &Cmdr)
{
//  int Dst;
//  if(Cmdr.getInt(Dst))
//  {
//    Worker.SendMsgVersionRequest(Dst);
//  }
//  else
//  {
//    Cmdr.println(F("Usage: GetVersion dst_dev_id"));
//    return false;
//  }
//
//  return true;
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

