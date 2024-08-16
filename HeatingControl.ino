

#include "global.h"
#include "src/Common_code/tApplication.h"
#include "src/Common_code/sensors/tSensorFactory.h"
#include "src/Common_code/sensors/tSensor.h"

class tHomeAutomation: public tApplication {
public:
	tHomeAutomation() : tApplication() {}

protected:

	virtual void AppSetupBefore() {
		// set default values for eeprom
		SetDefaultEEPromValues();
	}

	virtual void AppSetupAfter() {
		tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_SYSTEM_STATUS, SENSOR_ID_SYSTEM_STATUS,"SystemStatus",1,NULL,0,50,true);
	}
};

tHomeAutomation HomeAutomation;
