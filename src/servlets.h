#ifndef SERVLETS
#define SERVLETS

#include "lib/TCP_Communication_lib/httpServer.h"

class tjavaScriptServlet :  public tHttpServlet
{
public:
  tjavaScriptServlet() : tHttpServlet() {}
  virtual ~tjavaScriptServlet() {}

  virtual bool ProcessAndResponse();
};


class tHeatingControlServletTMP :  public tHttpServlet
{
public:
	tHeatingControlServletTMP() : tHttpServlet() {}
  virtual ~tHeatingControlServletTMP() {}

  virtual bool ProcessAndResponse();
};


#endif


