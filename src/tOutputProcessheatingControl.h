/*
 * tOutputProcessheatingControl.h
 *
 *  Created on: 9 paü 2022
 *      Author: szkud
 */

#ifndef TOUTPUTPROCESSHEATINGCONTROL_H_
#define TOUTPUTPROCESSHEATINGCONTROL_H_

#include "../global.h"
#include "Common_code/OutputProcess.h"

class tOutputProcess_heatingControl : public tOutputProcess {
public:
   tOutputProcess_heatingControl(Scheduler &manager) : tOutputProcess(manager) {}

   virtual void setup()
   {
      Output[OUT_ID_FLOOR_HEAT_VALVE_OPEN].SetPin(OUT_PIN_FLOOR_HEAT_VALVE_OPEN,0);
      Output[OUT_ID_FLOOR_HEAT_VALVE_CLOSE].SetPin(OUT_PIN_FLOOR_HEAT_VALVE_CLOSE,0);
      Output[OUT_ID_FLOOR_HEAT_PUMP].SetPin(OUT_PIN_FLOOR_HEAT_PUMP,0);
   }
};


extern tOutputProcess_heatingControl OutputProcess;

#endif /* TOUTPUTPROCESSHEATINGCONTROL_H_ */
