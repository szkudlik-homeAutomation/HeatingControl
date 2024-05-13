#ifndef SERVLETS_LOCAL
#define SERVLETS_LOCAL

#include "../global.h"
#include "Common_code/Network/httpServer.h"

class tHeatingControlServlet :  public tHttpServlet
{
public:
	tHeatingControlServlet() : tHttpServlet() {}
  virtual ~tHeatingControlServlet() {}

  virtual bool ProcessAndResponse();
};


#endif


