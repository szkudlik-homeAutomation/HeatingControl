/*
 * IncomingFrameHandler.h
 *
 *  Created on: 10 sie 2022
 *      Author: szkud
 */

#pragma once

#include "../global.h"
#include "Common_code/TLE8457_serial/tIncomingFrameHanlder.h"

class tHeatingCtrlIncomingFrameHandler : public tIncomingFrameHanlder {
public:
	tHeatingCtrlIncomingFrameHandler() : tIncomingFrameHanlder() { }

   virtual void onMessage(uint8_t type, uint16_t data, void *pData);

private:
   // handlers of all incoming frames

};
