#include "../global.h"

#if APP_Node

#include "Common_code/tApplication.h"

#include "Common_code/sensors/tSensorFactory.h"
#include "Common_code/sensors/tSensor.h"

#include "Common_code/sensors/tSystemStatusSensor.h"
#include "Common_code/sensors/tSht3Sensor.h"


class tHomeAutomation: public tApplication {
public:
	tHomeAutomation() : tApplication() {}

protected:

	virtual void AppSetupBefore() {
		// set default values for eeprom
		SetDefaultEEPromValues();
	}

	virtual void AppSetupAfter() {
		// system status, updated every 60s
		tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_SYSTEM_STATUS, SENSOR_ID_SYSTEM_STATUS,
					F("SystemStatus"),1,NULL,0,600,true, 1 << EV_TYPE_MEASUREMENT_COMPLETED);

		tSht3Sensor::tConfig tSht3SensorConfig;
		tSht3SensorConfig.I2C_Addr = SHT30_I2C_ADDR_44;
		tSht3SensorConfig.Avg = 1;
		tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_SHT3, 2,F("TempHum"),1,&tSht3SensorConfig,sizeof(tSht3SensorConfig),600,true,  1 << EV_TYPE_MEASUREMENT_COMPLETED);
	}
};

tHomeAutomation HomeAutomation;

#endif APP_Node
