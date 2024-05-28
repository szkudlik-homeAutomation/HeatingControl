#pragma once

#include "../../../global.h"
#if CONFIG_HEATING_CIRCLE_CONTROL_SERVLET

#include "../../Common_code/Network/httpServer.h"

class tHeatingControlServlet :  public tHttpServlet
{
public:
	tHeatingControlServlet() : tHttpServlet() {}
  virtual ~tHeatingControlServlet() {}

  virtual bool ProcessAndResponse();
};


#endif // CONFIG_HEATING_CIRCLE_CONTROL_SERVLET



