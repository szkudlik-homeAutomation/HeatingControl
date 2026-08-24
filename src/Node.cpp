#include "../global.h"

#if APP_Node

#include "Common_code/tApplication.h"

#include "Common_code/sensors/tSensorFactory.h"
#include "Common_code/sensors/tSensor.h"

#include "Common_code/sensors/tSystemStatusSensor.h"
#include "Common_code/sensors/tSht3Sensor.h"
#include "Common_code/sensors/tTgs2603AnalogSensor.h"
#include "Common_code/sensors/tDS1820Sensor.h"
#include "Common_code/sensors/tSimpleDigitalInputSensor.h"

#define SENSOR_ID_SYSTEM_STATUS 1
#define SENSOR_ID_SHT3 2
#define SENSOR_ID_TGS_2603 3
#define SENSOR_ID_DS1820 4
#define SENSOR_ID_GPIO_INPUTS 5

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

		// Humidity and temperature sensor SHT3, updated every 62s, with averaging of all measurements since last reading
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

		// Odour sensor TGS2603, updated every 10s
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

		// Temperature sensor DS1820, updated every 61s
		if (is_sensor_enabled(SensorsBitmap, SENSOR_ID_DS1820))
		{
		    tDS1820Sensor::tConfig DS1820config;
		    DS1820config.Avg = 0;
		    DS1820config.Pin = CONFIG_DS1820_PIN;
			tSensorFactory::Instance->CreateSensor(
					SENSOR_TYPE_DS1820,
					SENSOR_ID_DS1820,F("Temp"),1,
					&DS1820config,sizeof(DS1820config),
					610,	// 1 min 1 sec
					true, 1 << EV_TYPE_MEASUREMENT_COMPLETED);
		}

		// GPIO inputs sensors, scheduled every 630s, any change will trigger async event message
		if(is_sensor_enabled(SensorsBitmap, SENSOR_ID_GPIO_INPUTS))
		{
			tSimpleDigitalInputSensor::tConfig SimpleDigitalInputSensorConfig;
			uint8_t InputEnableBitmap = 0;
			uint8_t InputPolarityBitmap = 0;
			EEPROM.get(EEPROM_GPIO_INPUT_ENABLE_BITMAP_OFFSET, InputEnableBitmap);
			EEPROM.get(EEPROM_GPIO_INPUT_POLARITY_BITMAP_OFFSET, InputPolarityBitmap);

			uint8_t NumOfInputs = MIN(CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR_NUM_OF_INPUTS, 8); // max 8 inputs
			SimpleDigitalInputSensorConfig.NumOfInputs = 0;
			SimpleDigitalInputSensorConfig.ActiveStateBitmap = 0;
			
			for (uint8_t i=0; i<NumOfInputs; i++)
			{
				if (InputEnableBitmap & (1 << i))
				{
					switch (i)
					{	
						case 0: SimpleDigitalInputSensorConfig.Pin[SimpleDigitalInputSensorConfig.NumOfInputs] = CONFIG_OUTPUT_PROCESS_PIN0; break;
						case 1: SimpleDigitalInputSensorConfig.Pin[SimpleDigitalInputSensorConfig.NumOfInputs] = CONFIG_OUTPUT_PROCESS_PIN1; break;
						case 2: SimpleDigitalInputSensorConfig.Pin[SimpleDigitalInputSensorConfig.NumOfInputs] = CONFIG_OUTPUT_PROCESS_PIN2; break;
						case 3: SimpleDigitalInputSensorConfig.Pin[SimpleDigitalInputSensorConfig.NumOfInputs] = CONFIG_OUTPUT_PROCESS_PIN3; break;
						case 4: SimpleDigitalInputSensorConfig.Pin[SimpleDigitalInputSensorConfig.NumOfInputs] = CONFIG_OUTPUT_PROCESS_PIN4; break;
						case 5: SimpleDigitalInputSensorConfig.Pin[SimpleDigitalInputSensorConfig.NumOfInputs] = CONFIG_OUTPUT_PROCESS_PIN5; break;
					}

					SimpleDigitalInputSensorConfig.ActiveStateBitmap |= 
					 	(InputPolarityBitmap & (1 << i)) ? (1 << SimpleDigitalInputSensorConfig.NumOfInputs) : 0;
				
					SimpleDigitalInputSensorConfig.NumOfInputs++;
				}
			}

			if (SimpleDigitalInputSensorConfig.NumOfInputs > 0)
			{
				tSensorFactory::Instance->CreateSensor(
						SENSOR_TYPE_DIGITAL_INPUT,
						SENSOR_ID_GPIO_INPUTS,F("GPIOin"),2,
						&SimpleDigitalInputSensorConfig,sizeof(SimpleDigitalInputSensorConfig),
						630,	// 1 min shedule, any change will trigger async event message
						true, 1 << EV_TYPE_MEASUREMENT_COMPLETED || 1 << EV_TYPE_MEASUREMENT_CHANGE);
			}
		}
	}
};

tHomeAutomationHeatingNode HomeAutomationHeatingNode;

#endif APP_Node
