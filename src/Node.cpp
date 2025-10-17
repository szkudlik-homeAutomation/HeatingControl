#include "../global.h"

#if APP_Node

#include "Common_code/tApplication.h"

#include "Common_code/sensors/tSensorFactory.h"
#include "Common_code/sensors/tSensor.h"

#include "Common_code/sensors/tSystemStatusSensor.h"
#include "Common_code/sensors/tSht3Sensor.h"
#include "Common_code/sensors/tTgs2603AnalogSensor.h"

#define SENSOR_ID_SYSTEM_STATUS 1
#define SENSOR_ID_SHT3 2
#define SENSOR_ID_TGS_2603 3

#define is_sensor_enabled(_SensorsBitmap, _sensor_id) (_SensorsBitmap & (1 << ((_sensor_id)-1)))

class tHomeAutomationHeatingNode: public tApplication {
public:
	tHomeAutomationHeatingNode() : tApplication() {}

protected:

	virtual void AppSetupBefore() {
		// set default values for eeprom
		SetDefaultEEPromValues();
	}

	virtual void AppSetupAfter() {

		// Sensors setup, for home autimation sensors board

		uint32_t SensorsBitmap;
#if CONFIG_EEPROM_ENABLED_SENSORS
		EEPROM.get(EEPROM_SENSORS_BITMAP_OFFSET, SensorsBitmap);
#else //CONFIG_EEPROM_ENABLED_SENSORS
		SensorsBitmap = 0xFFFFFFFF; // all sensors enabled
#endif //CONFIG_EEPROM_ENABLED_SENSORS

		// system status, updated every 60s
		if (is_sensor_enabled(SensorsBitmap, SENSOR_ID_SYSTEM_STATUS))
		{
			tSensorFactory::Instance->CreateSensor(
					SENSOR_TYPE_SYSTEM_STATUS,
					SENSOR_ID_SYSTEM_STATUS,
					F("SystemStatus"),
					1,NULL,0,600,	// 1 min reporting
					true,
					1 << EV_TYPE_MEASUREMENT_COMPLETED);
		}


		if (is_sensor_enabled(SensorsBitmap, SENSOR_ID_SHT3))
		{
			tSht3Sensor::tConfig tSht3SensorConfig;
			tSht3SensorConfig.I2C_Addr = SHT30_I2C_ADDR_44;
			tSht3SensorConfig.Avg = 1;
			tSensorFactory::Instance->CreateSensor(
					SENSOR_TYPE_SHT3,
					SENSOR_ID_SHT3 ,F("TempHum"),1,
					&tSht3SensorConfig,sizeof(tSht3SensorConfig),620,true,	// 1 min 2 sec
					1 << EV_TYPE_MEASUREMENT_COMPLETED);
		}

		if (is_sensor_enabled(SensorsBitmap, SENSOR_ID_TGS_2603))
		{
			tTgs2603AnalogSensor::tConfig tTgs2603AnalogSensorConfig;
			tTgs2603AnalogSensorConfig.Pin = A0;
			tSensorFactory::Instance->CreateSensor(
					SENSOR_TYPE_TGS2603,
					SENSOR_ID_TGS_2603,F("Odour"),1,
					&tTgs2603AnalogSensorConfig,sizeof(tTgs2603AnalogSensorConfig),
					100,	// 10 sec reporting
					true, 1 << EV_TYPE_MEASUREMENT_COMPLETED);
		}

	}
};

tHomeAutomationHeatingNode HomeAutomationHeatingNode;

#endif APP_Node
