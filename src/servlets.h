#ifndef SERVLETS
#define SERVLETS

#include "Common_code/Network/httpServer.h"

class tjavaScriptServlet :  public tHttpServlet
{
public:
  tjavaScriptServlet() : tHttpServlet() {}
  virtual ~tjavaScriptServlet() {}

  virtual bool ProcessAndResponse();
};

#endif
