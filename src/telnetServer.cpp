#include "telnetServer.h"
#include "common.h"
//#include "WorkerProcess.h"

bool helloHandler(Commander &Cmdr);
bool enableLogs(Commander &Cmdr);
bool disableLogs(Commander &Cmdr);
bool send_GetVersion(Commander &Cmdr);
bool send_Reset(Commander &Cmdr);
bool trigger_ScanNodes(Commander &Cmdr);

// must be static-global (why? - only 1 telnet session may be active)
Commander cmd;

const commandList_t masterCommands[] = {
  {"enableLogs",      enableLogs,                   "enable logs on telnet console"},
  {"disableLogs",     disableLogs,                  "enable logs on telnet console"},
  {"Reset",           send_Reset,                   "reset the device"},
};


tTelnetSession *pTelnetSession = NULL;

tTelnetSession::tTelnetSession(EthernetClient aEthernetClient) : tTcpSession(aEthernetClient, TELNET_SESSION_TIMEOUT), ResponseHandler()
{
  DEBUG_PRINTLN_3("TELNET Session started");
  cmd.begin(&mEthernetClient, masterCommands, sizeof(masterCommands));
  cmd.commandPrompt(ON); //enable the command prompt
  cmd.echo(false);     //Echo incoming characters to theoutput port
  cmd.errorMessages(ON); //error messages are enabled - it will tell us if we issue any unrecognised commands
  cmd.autoChain(ON);
  cmd.printCommandList();
  pTelnetSession = this;
  DisableLogs();
}

bool tTelnetSession::doProcess()
{
  cmd.update();
  return true;
}

tTelnetSession::~tTelnetSession() { pTelnetSession = NULL; }

void tTelnetSession::vLog(uint8_t str)
{
    if (NULL != cmd.getOutputPort())
      cmd.getOutputPort()->write(str);
}

bool enableLogs(Commander &Cmdr)
{
  pTelnetSession->EnableLogs();
}

bool disableLogs(Commander &Cmdr)
{
  pTelnetSession->DisableLogs();
}

bool send_Reset(Commander &Cmdr)
{
//  Worker.doReset();
  return true;
}
