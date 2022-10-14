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
bool set_Params(Commander &Cmdr);
bool flush(Commander &Cmdr);


const commandList_t TelnetCommands[] = {
  {"enableLogs",      enableLogs,                   "enable logs on telnet console"},
  {"disableLogs",     disableLogs,                  "enable logs on telnet console"},
  {"GetVersion",      send_GetVersion,              "show version"},
  {"Reset",           send_Reset,                   "reset the system"},
  {"SetParams",      set_Params, "set_Params target delta1 delta2"},
  {"flush",      flush, "set_Params target fastThold tolerance"},
};

tTelnetServer TelnetServer(TelnetCommands,sizeof(TelnetCommands));


bool flush(Commander &Cmdr)
{
   for (int i = 0; i<50; i++)
   {
      Serial.println(F("Usage: ConfigureDS1820 SensorID Pin NumOfDevices Avg Name [Name1] [Name2]..."));
   }
   return true;
}

extern tHeatingCircleControl FloorTemperatureValveControl;

bool set_Params(Commander &Cmdr)
{
	float target;
	float fastThold;
	float tolerance;

   if(!Cmdr.getFloat(target))
   {
     goto error;
   }

   if(!Cmdr.getFloat(fastThold))
   {
     goto error;
   }

   if(!Cmdr.getFloat(tolerance))
   {
     goto error;
   }


   FloorTemperatureValveControl.setFastThold(fastThold);
   FloorTemperatureValveControl.setTargetTemp(target);
   FloorTemperatureValveControl.setTolerance(tolerance);

   Cmdr.println(F("DONE"));
   return true;

   error:
     Cmdr.println(F("Usage: set_Params target fastThold tolerance"));
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

