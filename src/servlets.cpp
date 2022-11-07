#include "servlets.h"
#include "tOutputProcessheatingControl.h"
#include "tHeatingCircleControl.h"


bool tjavaScriptServlet::ProcessAndResponse()
{
//	pOwner->SendFlashString(javascript_js_raw,javascript_js_raw_len);
//	return false;
}


extern tHeatingCircleControl *pFloorTemperatureValveControl;
extern tHeatingCircleControl *pRadiatorsTemperatureValveControl;


bool tHeatingControlServletTMP::ProcessAndResponse()
{
	uint16_t PowerFloor;
	uint16_t TempFloor;
	uint16_t PowerRadiators;
	uint16_t TempRadiators;

	if (GetParameter("PowerFloor",&PowerFloor))
	{
		if (PowerFloor)
			pFloorTemperatureValveControl->Start();
		else
			pFloorTemperatureValveControl->Stop();
	}

	if (GetParameter("PowerRadiators",&PowerRadiators))
	{
		if (PowerRadiators)
			pRadiatorsTemperatureValveControl->Start();
		else
			pRadiatorsTemperatureValveControl->Stop();
	}

	if (GetParameter("TempFloor",&TempFloor))
	{
		pFloorTemperatureValveControl->setTargetTemp((float)TempFloor/10);
	}

	if (GetParameter("TempRadiators",&TempRadiators))
	{
		pRadiatorsTemperatureValveControl->setTargetTemp((float)TempRadiators/10);
	}

    pOwner->SendFlashString(PSTR("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"));

    pOwner->SendFlashString(PSTR("Floor: "));
    if (pFloorTemperatureValveControl->isWorking())
    {
        pOwner->SendFlashString(PSTR("ON "));
    }
    else
    {
        pOwner->SendFlashString(PSTR("OFF "));
    }
    pOwner->SendFlashString(PSTR("Temperature set: "));
    pOwner->mEthernetClient.println(pFloorTemperatureValveControl->getTargetTemp());

    pOwner->SendFlashString(PSTR("Radiators: "));
    if (pRadiatorsTemperatureValveControl->isWorking())
    {
        pOwner->SendFlashString(PSTR("ON "));
    }
    else
    {
        pOwner->SendFlashString(PSTR("OFF "));
    }
    pOwner->SendFlashString(PSTR("Temperature set: "));
    pOwner->mEthernetClient.print(pRadiatorsTemperatureValveControl->getTargetTemp());

    return false;
}
