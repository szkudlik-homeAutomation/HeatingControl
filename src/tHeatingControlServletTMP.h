#ifndef SERVLETS_LOCAL
#define SERVLETS_LOCAL

#include "../global.h"
#include "Common_code/Network/httpServer.h"

class tHeatingControlServletTMP :  public tHttpServlet
{
public:
	tHeatingControlServletTMP() : tHttpServlet() {}
  virtual ~tHeatingControlServletTMP() {}

  virtual bool ProcessAndResponse();
};


#endif


