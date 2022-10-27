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
	uint16_t PowerFloor;  // device iD
	uint16_t TempFloor;  // device iD
	uint16_t PowerRadiators;  // device iD
	uint16_t TempRadiators;  // device iD

	if (GetParameter("PowerFloor",&PowerFloor))
	{
		if (PowerFloor)
			FloorTemperatureValveControl.Start();
		else
			FloorTemperatureValveControl.Stop();
	}

	if (GetParameter("PowerRadiators",&PowerRadiators))
	{
		if (PowerFloor)
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
		RadiatorsTemperatureValveControl.setTargetTemp((float)TempFloor/10);
	}

    pOwner->SendFlashString(PSTR("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"));

    pOwner->SendFlashString(PSTR("Podłoga: "));
    if (FloorTemperatureValveControl.isWorking())
    {
        pOwner->SendFlashString(PSTR("ON "));
    }
    else
    {
        pOwner->SendFlashString(PSTR("OFF "));
    }
    pOwner->SendFlashString(PSTR("Temperatura: "));
    pOwner->mEthernetClient.println(FloorTemperatureValveControl.getTargetTemp());

    pOwner->SendFlashString(PSTR("Kaloryfery: "));
    if (RadiatorsTemperatureValveControl.isWorking())
    {
        pOwner->SendFlashString(PSTR("ON "));
    }
    else
    {
        pOwner->SendFlashString(PSTR("OFF "));
    }
    pOwner->SendFlashString(PSTR("Temperatura: "));
    pOwner->mEthernetClient.print(RadiatorsTemperatureValveControl.getTargetTemp());


    return false;
}
