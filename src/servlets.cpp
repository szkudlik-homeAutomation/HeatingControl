#include "servlets.h"
#include "tOutputProcessheatingControl.h"
#include "Common_code/controllers/tHeatingCircleControl.h"


extern tHeatingCircleControl *pFloorTemperatureValveControl;
extern tHeatingCircleControl *pRadiatorsTemperatureValveControl;

// DEFINETLY not a god point for those macros
#define PUMP_DISABLE_THOLD_SHIFT -2
#define PUMP_ENABLE_THOLD_SHITF 0


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

    // calculate setPumpEnableTemp
    float RadiatorsTemp = -200;
    if (pRadiatorsTemperatureValveControl->isWorking())
       RadiatorsTemp = pRadiatorsTemperatureValveControl->getTargetTemp();

    float FloorTemp = -200;
    if (pFloorTemperatureValveControl->isWorking())
       FloorTemp = pFloorTemperatureValveControl->getTargetTemp();

   float MaxTemp = (FloorTemp > RadiatorsTemp) ? FloorTemp : RadiatorsTemp;

   tHeatingCircleControl::setPumpStopTemp(MaxTemp+PUMP_DISABLE_THOLD_SHIFT);
   tHeatingCircleControl::setPumpStartTemp(MaxTemp+PUMP_ENABLE_THOLD_SHITF);

   return false;
}
