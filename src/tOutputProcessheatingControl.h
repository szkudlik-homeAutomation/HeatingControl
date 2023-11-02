/*
 * tOutputProcessheatingControl.h
 *
 *  Created on: 9 pa� 2022
 *      Author: szkud
 */

#pragma once


#include "../global.h"
#include "Common_code/tOutputProcess.h"
#include "Common_code/Network/httpServer.h"

class tOutputProcess_heatingControl : public tOutputProcess {
public:
   tOutputProcess_heatingControl(Scheduler &manager) : tOutputProcess(manager) {}

   virtual void setup()
   {
      Output[OUT_ID_HEAT_PUMP_HEATING].SetPin(OUT_PIN_HEAT_PUMP_HEATING,0);
      Output[OUT_ID_COOLING].SetPin(OUT_PIN_COOLING,0);
      Output[OUT_ID_HI_HEAING].SetPin(OUT_PIN_HI_HEAING,0);
      Output[OUT_ID_RESERVED].SetPin(OUT_PIN_RESERVED,0);
      Output[OUT_ID_AIR_SPEED_2].SetPin(OUT_PIN_AIR_SPEED_2,0);
      Output[OUT_ID_AIR_SPEED_3].SetPin(OUT_PIN_AIR_SPEED_3,0);
      Output[OUT_ID_AIR_POWER_OFF].SetPin(OUT_PIN_AIR_POWER_OFF,0);
      Output[OUT_ID_AUX_WATER_HEATER].SetPin(OUT_PIN_AUX_WATER_HEATER,0);
      Output[OUT_ID_HOT_AIR].SetPin(OUT_PIN_HOT_AIR,0);
      Output[OUT_ID_FLOOR_PUMP].SetPin(OUT_PIN_FLOOR_PUMP,0);
      Output[OUT_ID_WATER_CIRCULATION_PUMP].SetPin(OUT_PIN_WATER_CIRCULATION_PUMP,0);
      Output[OUT_ID_READIATORS_PUMP].SetPin(OUT_PIN_READIATORS_PUMP,0);
      Output[OUT_ID_FLOOR_TEMP_LOWER].SetPin(OUT_PIN_FLOOR_TEMP_LOWER,0);
      Output[OUT_ID_RADIATOR_TEMP_LOWER].SetPin(OUT_PIN_RADIATOR_TEMP_LOWER,0);
      Output[OUT_ID_FLOOR_TEMP_HIGHER].SetPin(OUT_PIN_FLOOR_TEMP_HIGHER,0);
      Output[OUT_ID_RADIATOR_TEMP_HIGHER].SetPin(OUT_PIN_RADIATOR_TEMP_HIGHER,0);
   }
};
