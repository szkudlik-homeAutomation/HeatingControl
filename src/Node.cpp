#include "../global.h"

#if APP_Node

#include "Common_code/tApplication.h"

#include "Common_code/sensors/tSensorFactory.h"
#include "Common_code/sensors/tSensor.h"

#include "Common_code/sensors/tDS1820Sensor.h"
#include "Common_code/sensors/tImpulseSensor.h"
#include "Common_code/sensors/tPt100AnalogSensor.h"
#include "Common_code/sensors/tSimpleDigitalInputSensor.h"
#include "Common_code/sensors/tSystemStatusSensor.h"


class tHomeAutomation: public tApplication {
public:
	tHomeAutomation() : tApplication() {}

protected:

	virtual void AppSetupBefore() {
		// set default values for eeprom
		SetDefaultEEPromValues();
	}

	virtual void AppSetupAfter() {
	}
};

tHomeAutomation HomeAutomation;

#endif APP_Node
