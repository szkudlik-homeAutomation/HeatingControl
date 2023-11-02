/*
 * IncomingFrameHandler.cpp
 *
 *  Created on: 10 sie 2022
 *      Author: szkud
 */

#include "../global.h"
#include "tHeatingCtrlIncomingFrameHandler.h"

void tHeatingCtrlIncomingFrameHandler::onMessage(uint8_t type, uint16_t data, void *pData)
{
    uint8_t ret;

    ret = handleCommonMessages(data, pData);
    if (STATUS_SUCCESS == ret)
        // handled
        return;

    tCommunicationFrame *pFrame = (tCommunicationFrame *)pData;
    uint8_t SenderDevId = pFrame->SenderDevId;

    switch (data)	// messageType
      {

		 default:
			 DEBUG_PRINTLN_3("MESSAGE unknown type, drop");
      }
}
