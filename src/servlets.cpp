#include "servlets.h"
#include "http_binaries.h"

bool tjavaScriptServlet::ProcessAndResponse()
{
	pOwner->SendFlashString(javascript_js_raw,javascript_js_raw_len);
	return false;
}
