/*
 * tOutputProcessheatingControl.h
 *
 *  Created on: 9 pa� 2022
 *      Author: szkud
 */

#pragma once


#include "../global.h"
#include "Common_code/OutputProcess.h"
#include "Common_code/Network/httpServer.h"

class tOutputProcess_heatingControl : public tOutputProcess {
public:
   tOutputProcess_heatingControl(Scheduler &manager) : tOutputProcess(manager) {}

   virtual void setup()
   {
      Output[OUT_ID_TEST1].SetPin(OUT_PIN_TEST1,0);
      Output[OUT_ID_TEST2].SetPin(OUT_PIN_TEST2,0);
      Output[OUT_ID_TEST3].SetPin(OUT_PIN_TEST3,0);
   }
};

extern tOutputProcess_heatingControl OutputProcess;

