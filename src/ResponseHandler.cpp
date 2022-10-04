#include "ResponseHandler.h"

ResponseHandler* ResponseHandler::pFirst = NULL;
bool ResponseHandler::mLogsForced = false;

#ifdef DEBUG_SERIAL
ResponseHandlerSerial RespHandler;
#endif

ResponseHandler::~ResponseHandler()
{
  ResponseHandler *i = pFirst;
  ResponseHandler *prev = NULL;
  while(i != NULL)
  {
    if (i == this)
    {
      if (NULL == prev)        // first item?
      {
        pFirst = this->pNext; // next to this becomes first
      }
      else if (NULL == this->pNext) // last item?
      {
        prev->pNext = NULL;   // prev to this becomes last
      }
      else  // middle item
      {
        prev->pNext = this->pNext;
      }
      return; // done
    }
    prev = i;
    i = i->pNext;
  }
}

size_t ResponseHandler::write(uint8_t str)
{
	for (ResponseHandler * i = pFirst; i != NULL ; i = i->pNext)
	{
		if ((i->mLogEnbabled) | mLogsForced) i->vLog(str);
	}
}
