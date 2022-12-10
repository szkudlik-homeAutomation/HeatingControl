
#include "tHeatingControlServletTMP.h"
#include "tOutputProcessheatingControl.h"
#include "Common_code/controllers/tHeatingCircleControl.h"


extern tHeatingCircleControl *pFloorTemperatureValveControl;
extern tHeatingCircleControl *pRadiatorsTemperatureValveControl;

// DEFINETLY not a god point for those macros

// stop the pumps if heating water supply temp drops below target_temp + PUMP_DISABLE_THOLD_SHIFT
#define PUMP_DISABLE_THOLD_SHIFT -4

// never stop pumps if heating water supply temp is higher than PUMP_DISABLE_MAX_TEMP
#define PUMP_DISABLE_MAX_TEMP 36

// restart pump if heating storage temp raise higer than target_temp + PUMP_ENABLE_THOLD_SHIFT
#define PUMP_ENABLE_THOLD_SHIFT 0

// always restart pumps if heating water supply temp is higher than PUMP_ENABLE_PUMPS_FORCE
#define PUMP_FORCE_ENABLE_TEMP 36

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

    // get max from target temps
   float MaxTemp = (FloorTemp > RadiatorsTemp) ? FloorTemp : RadiatorsTemp;

   float PumpDisableTemp = MaxTemp + PUMP_DISABLE_THOLD_SHIFT;
   PumpDisableTemp = (PumpDisableTemp < PUMP_DISABLE_MAX_TEMP) ? PumpDisableTemp : PUMP_DISABLE_MAX_TEMP;

   float PumpEnableTemp = MaxTemp + PUMP_ENABLE_THOLD_SHIFT;
   PumpEnableTemp = (PumpEnableTemp < PUMP_FORCE_ENABLE_TEMP) ? PumpEnableTemp : PUMP_FORCE_ENABLE_TEMP;

   tHeatingCircleControl::setPumpStopTemp(PumpDisableTemp);
   tHeatingCircleControl::setPumpStartTemp(PumpEnableTemp);

   return false;
}
