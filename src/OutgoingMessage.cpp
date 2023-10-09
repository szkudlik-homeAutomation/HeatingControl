/*
 * OutgoingMessage.cpp
 *
 *  Created on: Jan 17, 2023
 *      Author: mszkudli
 */

#include "../global.h"
#include "OutgoingMessage.h"
#include "Common_code/TLE8457_serial/TLE8457_serial_lib.h"

// VERSION HANDSHAKE
bool OutgoingMessage::SendMsgVersionRequest(uint8_t RecieverID)
{
#if CONFIG_CENTRAL_NODE
  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_FW_VERSION_REQUEST");
  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_FW_VERSION_REQUEST,0,NULL);
#endif
  return true;
}

bool OutgoingMessage::SendMsgVersionResponse(uint8_t RecieverID, uint8_t Major, uint8_t Minor, uint8_t Patch)
{
  tMessageTypeFwVesionResponse Msg;
  Msg.Major = Major;
  Msg.Minor = Minor;
  Msg.Patch = Patch;

  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_FW_VERSION_RESPONSE,sizeof(Msg),&Msg);
};


//// OVERVIEW STATE HANDSHAKE
//bool OutgoingMessage::SendMsgOverviewStateRequest(uint8_t RecieverID)
//{
//#if CONFIG_CENTRAL_NODE
//  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_OVERVIEW_STATE_REQUEST");
//  CommSender.Enqueue(RecieverID, MESSAGE_TYPE_OVERVIEW_STATE_REQUEST, 0, NULL);
//#endif
//  return true;
//}
//
//bool OutgoingMessage::SendMsgOverviewStateResponse(uint8_t RecieverID, uint8_t  PowerState, uint8_t  TimerState)
//{
//  tMessageTypeOverviewStateResponse Msg;
//  Msg.PowerState = PowerState;
//  Msg.TimerState = TimerState;
//  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_OVERVIEW_STATE_RESPONSE");
//  CommSender.Enqueue(RecieverID, MESSAGE_TYPE_OVERVIEW_STATE_RESPONSE, sizeof(Msg), &Msg);
//  return true;
//}
//
//// GET OUTPUT STATE HANDSHAKE
//bool OutgoingMessage::SendMsgOutputStateRequest(uint8_t RecieverID, uint8_t  OutputID)
//{
//#if CONFIG_CENTRAL_NODE
//  tMessageTypeOutputStateRequest Msg;
//  Msg.OutputID = OutputID;
//  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_OUTPUT_STATE_REQUEST");
//  CommSender.Enqueue(RecieverID, MESSAGE_TYPE_OUTPUT_STATE_REQUEST, sizeof(Msg), &Msg);
//#endif
//  return true;
//};
//
//bool OutgoingMessage::SendMsgOutputStateResponse(uint8_t RecieverID, uint8_t  OutputID, uint8_t  PowerState, uint16_t TimerValue, uint16_t DefaultTimer)
//{
//  tMessageTypeOutputStateResponse Msg;
//  Msg.OutputID = OutputID;
//  Msg.PowerState = PowerState;
//  Msg.TimerValue = TimerValue;
//  Msg.DefaultTimer = DefaultTimer;
//  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_OUTPUT_STATE_RESPONSE");
//  CommSender.Enqueue(RecieverID, MESSAGE_TYPE_OUTPUT_STATE_RESPONSE, sizeof(Msg), &Msg);
//
//  return true;
//};
//
//// SET OUTPUT
//bool OutgoingMessage::SendMsgSetOutput(uint8_t RecieverID, uint8_t  OutId, uint8_t  State, uint16_t Timer)
//{
//#if CONFIG_CENTRAL_NODE
//  tMessageTypeSetOutput Message;
//  Message.OutId = OutId;
//  Message.State = State;
//  Message.Timer = Timer;
//  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_SET_OUTPUT");
//  CommSender.Enqueue(RecieverID, MESSAGE_TYPE_SET_OUTPUT, sizeof(Message), &Message);
//#endif
//  return true;
//}
//
//// BUTTON  PRESS
//bool OutgoingMessage::SendMsgButtonPress(uint8_t RecieverID, uint8_t ForceSrcId, uint16_t ShortClick, uint16_t LongClick, uint16_t DoubleClick)
//{
//  tMessageTypeButtonPress Msg;
//  Msg.ShortClick = ShortClick;
//  Msg.LongClick  = LongClick;
//  Msg.DoubleClick = DoubleClick;
//  Msg.ForceSrcId = ForceSrcId;
//
//  DEBUG_PRINTLN_3("===================>sending MESSAGE_BUTTON_PRESS");
//  CommSender.Enqueue(RecieverID,MESSAGE_BUTTON_PRESS,sizeof(Msg),&Msg);
//  return true;
//};
//
//bool OutgoingMessage::SendMsgClearAllActions(uint8_t RecieverID)
//{
//#if CONFIG_CENTRAL_NODE
//  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_CLEAR_ACTIONS");
//  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_CLEAR_ACTIONS,0,NULL);
//#endif
//  return true;
//};
//
//bool OutgoingMessage::SendMsgAddAction(uint8_t RecieverID, uint8_t OutId, uint8_t SenderDevID, uint8_t ButtonId, uint8_t TriggerType, uint8_t ActionType, uint16_t Timer, uint8_t  OutputsMask, uint8_t  OutputsStates)
//{
//#if CONFIG_CENTRAL_NODE
//  tMessageTypeSetAction Message;
//  Message.OutId = OutId;
//  Message.SenderDevID = SenderDevID;
//  Message.ButtonId = ButtonId;
//  Message.TriggerType = TriggerType;
//  Message.ActionType = ActionType;
//  Message.Timer = Timer;
//  Message.OutputsMask = OutputsMask;
//  Message.OutputsStates = OutputsStates;
//
//  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_SET_ACTION");
//  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_SET_ACTION,sizeof(Message),&Message);
//#endif
//  return true;
//};
//
//// EEPROM CRC Handshake
//bool OutgoingMessage::SendMsgEepromCrcRequest(uint8_t RecieverID)
//{
//#if CONFIG_CENTRAL_NODE
//  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_EEPROM_CRC_REQUEST");
//  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_EEPROM_CRC_REQUEST,0,NULL);
//#endif
//  return true;
//}
//
//bool OutgoingMessage::SendMsgEepromCrcResponse(uint8_t RecieverID,  uint8_t NumOfActions, uint16_t EepromCRC)
//{
//  tMessageTypeEepromCRCResponse Msg;
//  Msg.NumOfActions = NumOfActions;
//  Msg.EepromCRC = EepromCRC;
//  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_EEPROM_CRC_RESPONSE");
//  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_EEPROM_CRC_RESPONSE,sizeof(Msg),&Msg);
//  return true;
//};
//
//
//bool OutgoingMessage::SendMsgReset(uint8_t RecieverID)
//{
//  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_FORCE_RESET");
//  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_FORCE_RESET,0,NULL);
//}
//
//
//bool OutgoingMessage::SendMsgSetDefaultTimer(uint8_t RecieverID, uint8_t OutputID, uint16_t DefTimerValue)
//{
//  tMessageTypeSetDefaultTimer Msg;
//  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_SET_DEFAULT_TIMER");
//  Msg.OutputID = OutputID;
//  Msg.DefaultTimerValue = DefTimerValue;
//  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_SET_DEFAULT_TIMER,sizeof(Msg),&Msg);
//}
//
//bool OutgoingMessage::SendMsgDefaultTimerRequest(uint8_t RecieverID, uint8_t OutputID)
//{
//  tMessageTypeDefaultTimerRequest Msg;
//  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_DEFAULT_TIMER_REQUEST");
//  Msg.OutputID = OutputID;
//  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_DEFAULT_TIMER_REQUEST,sizeof(Msg),&Msg);
//}
//
//bool OutgoingMessage::SendMsgDefaultTimerResponse(uint8_t RecieverID, uint8_t OutputID, uint16_t DefTimerValue)
//{
//  tMessageTypeDefaultTimerResponse Msg;
//  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_DEFAULT_TIMER_RESPONSE");
//  Msg.OutputID = OutputID;
//  Msg.DefTimerValue = DefTimerValue;
//  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_DEFAULT_TIMER_RESPONSE,sizeof(Msg),&Msg);
//}
//
