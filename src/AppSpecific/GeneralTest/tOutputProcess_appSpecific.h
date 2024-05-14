/*
 * tOutputProcess_appSpecific.h
 *
 */

#pragma once

#include "../../../global.h"
#include "../../Common_code/tOutputProcess.h"

class tOutputProcess_appSpecific : public tOutputProcess {
public:
    tOutputProcess_appSpecific(Scheduler &manager) : tOutputProcess(manager) {}

   virtual void setup()
   {
      Output[OUT_ID_TEST1].SetPin(OUT_PIN_TEST1,0);
      Output[OUT_ID_TEST2].SetPin(OUT_PIN_TEST2,0);
      Output[OUT_ID_TEST3].SetPin(OUT_PIN_TEST3,0);
   }
};
