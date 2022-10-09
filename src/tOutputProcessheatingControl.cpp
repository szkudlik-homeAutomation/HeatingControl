/*
 * tOutputProcessheatingControl.cpp
 *
 *  Created on: 9 paü 2022
 *      Author: szkud
 */



#include "../global.h"
#include "tOutputProcessheatingControl.h"

bool tOutputSetServlet::ProcessAndResponse()
{
   uint16_t Output;  // output iD
   uint16_t State;  // 1 - on, 0 - off
   uint16_t Timer = 0; // TODO DEFAULT_TIMER;
   bool ParametersOK = true;

   ParametersOK &= GetParameter("Out",&Output);
   ParametersOK &= GetParameter("State",&State);
   GetParameter("Timer",&Timer);    // optional
   ParametersOK &= (Output < NUM_OF_OUTPUTS);

   if (! ParametersOK)
   {
     SendResponse400();
     return false;
   }

   #ifdef DEBUG_3
      RespHandler.print(F("==>HTTP set output, Out="));
      RespHandler.print(Output,DEC);
      RespHandler.print(F(" State="));
      RespHandler.print(State,DEC);
      RespHandler.print(F(" Timer="));
      RespHandler.println(Timer,DEC);
   #endif

   OutputProcess.SetOutput(Output,State,Timer,tOutputProcess::ForceTimer);
   SendResponse200();

return false;
}



bool tOutputStateServlet::ProcessAndResponse()
{
   uint16_t Output;  // output iD
   uint8_t State;
   uint16_t TimerValue;
   uint16_t DefaultTimer = 1000;
   bool ParametersOK = true;

   ParametersOK &= GetParameter("Out",&Output);
   ParametersOK &= (Output < NUM_OF_OUTPUTS);

   if (! ParametersOK)
   {
     SendResponse400();
     return false;
   }

   State = OutputProcess.GetOutputState(Output);
   TimerValue = OutputProcess.GetOutputTimer(Output);


   #ifdef DEBUG_3
      RespHandler.print(F("==>HTTP output state get, Out="));
      RespHandler.print(Output,DEC);
      RespHandler.print(F(" State="));
      RespHandler.print(State,DEC);
      RespHandler.print(F(" Timer="));
      RespHandler.print(TimerValue,DEC);
      RespHandler.print(F(" DefaultTimer="));
      RespHandler.println(DefaultTimer,DEC);

   #endif

//   // format JSON response
   pOwner->SendFlashString(PSTR("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"Output\": "));
   pOwner->mEthernetClient.print(Output,DEC);
   pOwner->SendFlashString(PSTR(", \"State\": "));
   pOwner->mEthernetClient.print(State,DEC);
   pOwner->SendFlashString(PSTR(", \"Timer\": "));
   pOwner->mEthernetClient.print(TimerValue,DEC);
   pOwner->SendFlashString(PSTR(", \"DefaultTimer\": "));
   pOwner->mEthernetClient.print(DefaultTimer,DEC);
   pOwner->SendFlashString(PSTR("}\r\n"));

   return false;
}

