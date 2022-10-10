/*
 * telnetServerCommands.cpp
 *
 *  Created on: 6 paü 2022
 *      Author: szkud
 */

#include "../global.h"
#include <Commander.h>
#include "Common_code/telnetServer.h"

extern int16_t Target;

// valve servo at full speed
extern int16_t Delta1;

// valve at half speed till delta 2

// no correction
extern int16_t Delta2;

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
  {"flush",      flush, "set_Params target delta1 delta2"},
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


bool set_Params(Commander &Cmdr)
{
   int aTarget;
   int aDelta1;
   int aDelta2;

   if(!Cmdr.getInt(aTarget))
   {
     goto error;
   }

   if(!Cmdr.getInt(aDelta1))
   {
     goto error;
   }

   if(!Cmdr.getInt(aDelta2))
   {
     goto error;
   }

   Target = aTarget;
   Delta1 = aDelta1;
   Delta2 = aDelta2;
   Cmdr.println(F("DONE"));
   return true;

   error:
     Cmdr.println(F("Usage: ConfigureDS1820 SensorID Pin NumOfDevices Avg Name [Name1] [Name2]..."));
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

