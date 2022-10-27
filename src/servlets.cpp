#include "servlets.h"
#include "tOutputProcessheatingControl.h"
#include "tHeatingCircleControl.h"


bool tjavaScriptServlet::ProcessAndResponse()
{
//	pOwner->SendFlashString(javascript_js_raw,javascript_js_raw_len);
//	return false;
}


extern tHeatingCircleControl FloorTemperatureValveControl;
extern tHeatingCircleControl RadiatorsTemperatureValveControl;


bool tHeatingControlServletTMP::ProcessAndResponse()
{
	uint16_t PowerFloor;
	uint16_t TempFloor;
	uint16_t PowerRadiators;
	uint16_t TempRadiators;

	if (GetParameter("PowerFloor",&PowerFloor))
	{
		if (PowerFloor)
			FloorTemperatureValveControl.Start();
		else
			FloorTemperatureValveControl.Stop();
	}

	if (GetParameter("PowerRadiators",&PowerRadiators))
	{
		if (PowerRadiators)
			RadiatorsTemperatureValveControl.Start();
		else
			RadiatorsTemperatureValveControl.Stop();
	}

	if (GetParameter("TempFloor",&TempFloor))
	{
		FloorTemperatureValveControl.setTargetTemp((float)TempFloor/10);
	}

	if (GetParameter("TempRadiators",&TempRadiators))
	{
		RadiatorsTemperatureValveControl.setTargetTemp((float)TempRadiators/10);
	}

    pOwner->SendFlashString(PSTR("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"));

    pOwner->SendFlashString(PSTR("Floor: "));
    if (FloorTemperatureValveControl.isWorking())
    {
        pOwner->SendFlashString(PSTR("ON "));
    }
    else
    {
        pOwner->SendFlashString(PSTR("OFF "));
    }
    pOwner->SendFlashString(PSTR("Temperature set: "));
    pOwner->mEthernetClient.println(FloorTemperatureValveControl.getTargetTemp());

    pOwner->SendFlashString(PSTR("Radiators: "));
    if (RadiatorsTemperatureValveControl.isWorking())
    {
        pOwner->SendFlashString(PSTR("ON "));
    }
    else
    {
        pOwner->SendFlashString(PSTR("OFF "));
    }
    pOwner->SendFlashString(PSTR("Temperature set: "));
    pOwner->mEthernetClient.print(RadiatorsTemperatureValveControl.getTargetTemp());

    return false;
}
