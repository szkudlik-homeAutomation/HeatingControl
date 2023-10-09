/*
 * OutgoingMessage.h
 *
 *  Created on: Jan 17, 2023
 *      Author: mszkudli
 */

#pragma once

#include "../global.h"

class OutgoingMessage {
public:
    static bool SendMsgVersionRequest(uint8_t RecieverID);
    static bool SendMsgVersionResponse(uint8_t RecieverID, uint8_t Major, uint8_t Minor, uint8_t Patch);

    //	  static bool SendMsgOverviewStateRequest(uint8_t RecieverID);
//	  static bool SendMsgOverviewStateResponse(uint8_t RecieverID, uint8_t  PowerState, uint8_t  TimerState);
//	  static bool SendMsgOutputStateRequest(uint8_t RecieverID, uint8_t  OutputID);
//	  static bool SendMsgOutputStateResponse(uint8_t RecieverID, uint8_t  OutputID, uint8_t  PowerState, uint16_t TimerValue, uint16_t DefaultTimer);
//	  static bool SendMsgSetOutput(uint8_t RecieverID, uint8_t  OutId, uint8_t  State, uint16_t Timer);
//	  static bool SendMsgButtonPress(uint8_t RecieverID, uint8_t ForceSrcId, uint16_t ShortClick, uint16_t LongClick, uint16_t DoubleClick);
//	  static bool SendMsgAddAction(uint8_t RecieverID, uint8_t OutId, uint8_t SenderDevID, uint8_t ButtonId, uint8_t TriggerType, uint8_t ActionType, uint16_t Timer, uint8_t  OutputsMask, uint8_t  OutputsStates);
//	  static bool SendMsgClearAllActions(uint8_t RecieverID);
//	  static bool SendMsgEepromCrcRequest(uint8_t RecieverID);
//	  static bool SendMsgEepromCrcResponse(uint8_t RecieverID,  uint8_t NumOfActions, uint16_t EepromCRC);
//	  static bool SendMsgReset(uint8_t RecieverID);
//	  static bool SendMsgSetDefaultTimer(uint8_t RecieverID, uint8_t OutputID, uint16_t DefTimerValue);
//	  static bool SendMsgDefaultTimerRequest(uint8_t RecieverID,uint8_t OutputID);
//	  static bool SendMsgDefaultTimerResponse(uint8_t RecieverID, uint8_t OutputID, uint16_t DefTimerValue);
};
