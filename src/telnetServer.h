#ifndef TELNET_SERVER
#define TELNET_SERVER
#include "common.h"
#include "lib/TCP_Communication_lib/tcpServer.h"
#include "ResponseHandler.h"
#include <Commander.h>

class tTelnetSession : public tTcpSession, public ResponseHandler
{
public:
  tTelnetSession(EthernetClient aEthernetClient);
  virtual ~tTelnetSession();

protected:
  virtual bool doProcess();

  virtual void vLog(uint8_t str);
private:
  static const uint16_t TELNET_SESSION_TIMEOUT = 65535; // 10 minutes
};



class tTelnetServer : public tTcpServer
{
public:
  tTelnetServer() : tTcpServer(23) {}

protected:
  virtual tTcpSession* NewSession(EthernetClient aEthernetClient) { return new tTelnetSession(aEthernetClient); }
};



#endif  // WORKER_PROCESS
