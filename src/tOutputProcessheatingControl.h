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
#include "lib/TCP_Communication_lib/httpServer.h"

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


/**
 * output servlet - controlling or checking state of an output
 */
class tOutputSetServlet :  public tHttpServlet
{
public:
  tOutputSetServlet() : tHttpServlet() {}
  virtual ~tOutputSetServlet() {}

  virtual bool ProcessAndResponse();
};

class tOutputStateServlet : public tHttpServlet
{
  public:
   tOutputStateServlet() :  tHttpServlet() {};
   virtual ~tOutputStateServlet() {}

   virtual bool ProcessAndResponse();
};



extern tOutputProcess_heatingControl OutputProcess;

#endif /* TOUTPUTPROCESSHEATINGCONTROL_H_ */
