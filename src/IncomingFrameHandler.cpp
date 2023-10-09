/*
 * IncomingFrameHandler.cpp
 *
 *  Created on: 10 sie 2022
 *      Author: szkud
 */

#include "../global.h"
#include "IncomingFrameHandler.h"
#include "OutgoingMessage.h"

void tIncomingFrameHandler::onMessage(uint8_t type, uint16_t data, void *pData)
{
	tCommunicationFrame *pFrame = (tCommunicationFrame *)pData;
	uint8_t SenderDevId = pFrame->SenderDevId;

	switch (data)	// messageType
      {
	       case MESSAGE_TYPE_FW_VERSION_REQUEST:
	             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_FW_VERSION_REQUEST");
	             HandleMsgVersionRequest(SenderDevId);
	           break;

	       case MESSAGE_TYPE_FW_VERSION_RESPONSE:
	             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_FW_VERSION_RESPONSE");
	             HandleMsgVersionResponse(SenderDevId,(tMessageTypeFwVesionResponse*)(pFrame->Data));
	           break;


//       case MESSAGE_TYPE_OVERVIEW_STATE_REQUEST:
//           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_OVERVIEW_STATE_REQUEST");
//           HandleMsgOverviewStateRequest(SenderDevId);
//           break;
//
//       case MESSAGE_TYPE_OVERVIEW_STATE_RESPONSE:
//           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_OVERVIEW_STATE_RESPONSE");
//           HandleMsgOverviewStateResponse(SenderDevId,(tMessageTypeOverviewStateResponse*) (pFrame->Data));
//           break;
//
//       case MESSAGE_TYPE_OUTPUT_STATE_REQUEST:
//           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_OUTPUT_STATE_REQUEST");
//           HandleMsgOutputStateRequest(SenderDevId,(tMessageTypeOutputStateRequest*)(pFrame->Data));
//           break;
//
//       case MESSAGE_TYPE_OUTPUT_STATE_RESPONSE:
//           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_OUTPUT_STATE_RESPONSE");
//           HandleMsgOutputStateResponse(SenderDevId,(tMessageTypeOutputStateResponse*) (pFrame->Data));
//           break;
//
//       case MESSAGE_TYPE_SET_OUTPUT:
//           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_SET_OUTPUT");
//           HandleMsgSetOutput(SenderDevId,(tMessageTypeSetOutput*)(pFrame->Data));
//           break;
//
//       case MESSAGE_BUTTON_PRESS:
//           DEBUG_PRINTLN_3("===================>MESSAGE_BUTTON_PRESS");
//           HandleMsgButtonPress(SenderDevId, (tMessageTypeButtonPress*)(pFrame->Data));
//           break;
//
//       case MESSAGE_TYPE_SET_ACTION:
//             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_SET_ACTION");
//             HandleMsgSetAction(SenderDevId, (tMessageTypeSetAction*)(pFrame->Data));
//           break;
//
//       case MESSAGE_TYPE_CLEAR_ACTIONS:
//             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_CLEAR_ACTIONS");
//             HandleMsgClearAllActions(SenderDevId);
//           break;
//
//       case MESSAGE_TYPE_EEPROM_CRC_REQUEST:
//             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_EEPROM_CRC_REQUEST");
//             HandleMsgEepromCrcRequest(SenderDevId);
//           break;
//
//       case MESSAGE_TYPE_EEPROM_CRC_RESPONSE:
//             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_EEPROM_CRC_RESPONSE");
//             HandleMsgEepromCrcResponse(SenderDevId,(tMessageTypeEepromCRCResponse*)(pFrame->Data));
//           break;
//
//       case MESSAGE_TYPE_FORCE_RESET:
//             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_FORCE_RESET");
//             cli();
//             while(1); // let watchdog reboot the device
//           break;
//
//       case MESSAGE_TYPE_SET_DEFAULT_TIMER:
//             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_SET_DEFAULT_TIMER");
//             HandleMsgSetDefaultTimer(SenderDevId,(tMessageTypeSetDefaultTimer*)(pFrame->Data));
//           break;
//
//       case MESSAGE_TYPE_DEFAULT_TIMER_REQUEST:
//             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_DEFAULT_TIMER_REQUEST");
//             HandleMsgDefaultTimerRequest(SenderDevId,(tMessageTypeDefaultTimerRequest*)(pFrame->Data));
//           break;
//
//       case MESSAGE_TYPE_DEFAULT_TIMER_RESPONSE:
//             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_DEFAULT_TIMER_RESPONSE");
//             HandleMsgDefaultTimerResponse(SenderDevId,(tMessageTypeDefaultTimerResponse*)(pFrame->Data));
//           break;
//
     default:
         DEBUG_PRINTLN_3("MESSAGE  unknown type, drop");
      }
}

void tIncomingFrameHandler::HandleMsgVersionRequest(uint8_t SenderID)
{
    OutgoingMessage::SendMsgVersionResponse(SenderID,FW_VERSION_MAJOR,FW_VERSION_MINOR,FW_VERSION_PATCH);
}


void tIncomingFrameHandler::HandleMsgVersionResponse(uint8_t SenderID, tMessageTypeFwVesionResponse *Message)
{
#if CONFIG_CENTRAL_NODE
    tMessages::VersionResponseHandler(SenderID,Message->Major,Message->Minor,Message->Patch);
#endif
}


//void tIncomingFrameHandler::HandleMsgOverviewStateRequest(uint8_t SenderID)
//{
//   OutgoingMessage::SendMsgOverviewStateResponse(SenderID,OutputProcess.GetOutputStateMap(),OutputProcess.GetOutputTimersStateMap());
//}
//
//
//void tIncomingFrameHandler::HandleMsgOverviewStateResponse(uint8_t SenderID, tMessageTypeOverviewStateResponse* Message)
//{
//#if CONFIG_CENTRAL_NODE
//	LightControlMessages::OverviewStateResponseHandler(SenderID,Message->PowerState,Message->TimerState);
//#endif
//}
//
//void tIncomingFrameHandler::HandleMsgOutputStateRequest(uint8_t SenderID, tMessageTypeOutputStateRequest* Message)
//{
//  if (Message->OutputID < NUM_OF_OUTPUTS)
//  {
//      uint16_t DefTimer;
//      EEPROM.get(EEPROM_DEFAULT_TIMER_VALUE_OFFSET+Message->OutputID*(sizeof(uint16_t)),DefTimer);
//
//      OutgoingMessage::SendMsgOutputStateResponse(SenderID,Message->OutputID, OutputProcess.GetOutputState(Message->OutputID), OutputProcess.GetOutputTimer(Message->OutputID),DefTimer);
//  }
//}
//
//void tIncomingFrameHandler::HandleMsgOutputStateResponse(uint8_t SenderID, tMessageTypeOutputStateResponse* Message)
//{
//#if CONFIG_CENTRAL_NODE
//	LightControlMessages::OutputStateResponseHandler(SenderID,Message->OutputID,Message->PowerState,Message->TimerValue,Message->DefaultTimer);
//#endif
//}
//
//void tIncomingFrameHandler::HandleMsgSetOutput(uint8_t SenderID, tMessageTypeSetOutput* Message)
//{
//   if (Message->OutId >= NUM_OF_OUTPUTS)
//   {
//     // drop it
//     return;
//   }
//   uint16_t Timer = Message->Timer;
//   if (DEFAULT_TIMER == Timer)
//   {
//     EEPROM.get(EEPROM_DEFAULT_TIMER_VALUE_OFFSET+Message->OutId*(sizeof(uint16_t)),Timer);
//   }
//
//  OutputProcess.SetOutput(Message->OutId,Message->State,Timer,tOutputProcess::ForceTimer);
//}
//
//
//void tIncomingFrameHandler::HandleMsgButtonPress(uint8_t SenderID, tMessageTypeButtonPress *Message)
//{
//#if CONFIG_CENTRAL_NODE
//	DEBUG_PRINT_3("Dev ID:");
//	DEBUG_3(print(SenderID,HEX));
//	DEBUG_PRINT_3(" ForcedSrc:");
//	DEBUG_3(print(Message->ForceSrcId,HEX));
//    DEBUG_PRINT_3(" short:");
//    DEBUG_3(print(Message->ShortClick,BIN));
//    DEBUG_PRINT_3(" long:");
//    DEBUG_3(print(Message->LongClick,BIN));
//    DEBUG_PRINT_3(" dbl:");
//    DEBUG_3(println(Message->DoubleClick,BIN));
//#endif
//
//  if (Message->ForceSrcId)
//    SenderID = Message->ForceSrcId;
//
//  // iterate through the action table
//  tMessageTypeSetAction Action;
//  uint8_t i = EEPROM.read(EEPROM_ACTION_TABLE_USAGE_OFFSET);
//  // remeber output state BEFORE performing action set
//  uint8_t OutputState = OutputProcess.GetOutputStateMap();
//  while (i--)
//  {
//    EEPROM.get(EEPROM_ACTION_TABLE_OFFSET+(EEPROM_ACTION_TABLE_SIZE*i),Action);
//    // does the sender ID match?
//    if (Action.SenderDevID != SenderID) continue;
//
//    // does the action type and button ID match?
//    uint16_t ButtonMask = (1 << Action.ButtonId);
//    if ( (BUTTON_TRIGGER_TYPE_ANY_CLICK == Action.TriggerType)    && (0 == (Message->ShortClick & ButtonMask)) && (0 == (Message->DoubleClick & ButtonMask))) continue;
//
//    if ( (BUTTON_TRIGGER_TYPE_CLICK == Action.TriggerType)        && (0 == (Message->ShortClick & ButtonMask))) continue;
//
//    if ( (BUTTON_TRIGGER_TYPE_LONG_CLICK == Action.TriggerType)   && (0 == (Message->LongClick & ButtonMask))) continue;
//
//    if ( (BUTTON_TRIGGER_TYPE_DOUBLE_CLICK == Action.TriggerType) && (0 == (Message->DoubleClick & ButtonMask))) continue;
//
//    // does the current output state match the mask?
//    uint8_t OutputStateMasked = OutputState & Action.OutputsMask;
//    if (OutputStateMasked != Action.OutputsStates) continue;
//
//    if (Action.OutId  >= NUM_OF_OUTPUTS)
//    {
//      // drop it
//      return;
//    }
//
//    uint16_t Timer = Action.Timer;
//    if (DEFAULT_TIMER == Timer)
//    {
//      EEPROM.get(EEPROM_DEFAULT_TIMER_VALUE_OFFSET+Action.OutId*(sizeof(uint16_t)),Timer);
//    }
//
//    switch (Action.ActionType)
//    {
//      case BUTTON_ACTION_TYPE_ON:
//         // when "turn on" action is triggered bu a button, don't set a timer if it is shorter than current timer
//        OutputProcess.SetOutput(Action.OutId,1,Timer,tOutputProcess::TimerLongerOnly);
//        break;
//
//      case BUTTON_ACTION_TYPE_OFF:
//        OutputProcess.SetOutput(Action.OutId,0,0,tOutputProcess::ForceTimer);
//        break;
//
//      case BUTTON_ACTION_TYPE_TOGGLE:
//        OutputProcess.ToggleOutput(Action.OutId,Timer);
//        break;
//    }
//  }
//}
//
//
//void tIncomingFrameHandler::HandleMsgClearAllActions(uint8_t SenderID)
//{
//    EEPROM.write(EEPROM_ACTION_TABLE_USAGE_OFFSET,0);
//}
//
//
//void tIncomingFrameHandler::HandleMsgSetAction(uint8_t SenderID, tMessageTypeSetAction* Message)
//{
//  uint8_t ActionTableUsage = EEPROM.read(EEPROM_ACTION_TABLE_USAGE_OFFSET);
//  if (ActionTableUsage < ACTION_TABLE_SIZE)
//  {
//    EEPROM.put(EEPROM_ACTION_TABLE_OFFSET+(EEPROM_ACTION_TABLE_SIZE*ActionTableUsage),*Message);
//    ActionTableUsage++;
//    EEPROM.write(EEPROM_ACTION_TABLE_USAGE_OFFSET,ActionTableUsage);
//  }
//}
//
//
//void tIncomingFrameHandler::HandleMsgEepromCrcRequest(uint8_t SenderID)
//{
//
//  int NumOfActions = EEPROM.read(EEPROM_ACTION_TABLE_USAGE_OFFSET);
//  tMessageTypeSetAction Action;
//  crc_t crc = crc_init();
//  uint8_t i = NumOfActions;
//  while (i--)
//  {
//    EEPROM.get(EEPROM_ACTION_TABLE_OFFSET+(EEPROM_ACTION_TABLE_SIZE*i),Action);
//    crc = crc_update(crc, &Action, sizeof(Action));
//  }
//  crc = crc_finalize(crc);
//  OutgoingMessage::SendMsgEepromCrcResponse(SenderID,NumOfActions,crc);
//}
//
//
//void tIncomingFrameHandler::HandleMsgEepromCrcResponse(uint8_t SenderID, tMessageTypeEepromCRCResponse* Message)
//{
//#if CONFIG_CENTRAL_NODE
//	LightControlMessages::EepromCRCResponseHandler(SenderID,Message->NumOfActions,Message->EepromCRC);
//#endif
//}
//
//
//void tIncomingFrameHandler::HandleMsgSetDefaultTimer(uint8_t SenderID, tMessageTypeSetDefaultTimer *Message)
//{
//  if (Message->OutputID >= NUM_OF_OUTPUTS) return;
//  EEPROM.put(EEPROM_DEFAULT_TIMER_VALUE_OFFSET+Message->OutputID*(sizeof(uint16_t)),Message->DefaultTimerValue);
//}
//
//void tIncomingFrameHandler::HandleMsgDefaultTimerRequest(uint8_t SenderID, tMessageTypeDefaultTimerRequest *Message)
//{
//  if (Message->OutputID >= NUM_OF_OUTPUTS) return;
//  uint16_t DefTimer;
//  EEPROM.get(EEPROM_DEFAULT_TIMER_VALUE_OFFSET+Message->OutputID*(sizeof(uint16_t)),DefTimer);
//  OutgoingMessage::SendMsgDefaultTimerResponse(SenderID,Message->OutputID,DefTimer);
//}
//
//void tIncomingFrameHandler::HandleMsgDefaultTimerResponse(uint8_t SenderID, tMessageTypeDefaultTimerResponse *Message)
//{
//#if CONFIG_CENTRAL_NODE
//	LightControlMessages::DefaultTimerResponseHandler(SenderID,Message->OutputID,Message->DefTimerValue);
//#endif
//}
//
