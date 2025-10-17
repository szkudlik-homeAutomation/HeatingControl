#include "../global.h"

#if APP_HeatingCentral

#include "Common_code/tApplication.h"


#include "HeatingCircle/tHeatingCircleStatusSensor.h"
#include "HeatingCircle/tHeatingCircleControl.h"
#include "HeatingCircle/tHeatingControlServlet.h"

#include "Common_code/sensors/tSensorFactory.h"
#include "Common_code/sensors/tSensor.h"
#include "Common_code/Network/HttpServer.h"

#include "Common_code/sensors/tDS1820Sensor.h"
#include "Common_code/sensors/tImpulseSensor.h"
#include "Common_code/sensors/tPt100AnalogSensor.h"
#include "Common_code/sensors/tSimpleDigitalInputSensor.h"
#include "Common_code/sensors/tSystemStatusSensor.h"


// todo: make a general code, move home specific to side branch
#define SENSOR_ID_SYSTEM_STATUS 1

#define SENSOR_ID_1820_HEATING_TEMP 2
#define SENSOR_ID_1820_AIR_HUW_TEMP 3
#define SENSOR_ID_1820_OUTDOOR_TEMP 4

#define SENSOR_ID_IMPULSE_HEATPUMP        5
#define SENSOR_ID_IMPULSE_WATER_HEATER    6

#define SENSOR_ID_PT100_HOTAIR            7

#define SENSOR_ID_DIGITAL_RADIATORS_PUMP_CONTROL   8
#define SENSOR_ID_DIGITAL_WATER_HEATER_REQUEST     9
#define SENSOR_ID_DIGITAL_AUX                      10

#define SENSOR_ID_OUTPUT_STATES                    11

#define SENSOR_ID_RADIATORS_HEATING_STATE          12
#define SENSOR_ID_FLOOR_HEATING_STATE              13



/* sensor factory for application */
class tSensorFactory_app : public tSensorFactory {
public:
    tSensorFactory_app() : tSensorFactory() {}
protected:
	virtual tSensor *appSpecificCreateSensor(uint8_t SensorType, uint8_t SensorID)
	{
	    switch (SensorType)
	    {
	    case SENSOR_TYPE_HEATING_CIRCLE_STATE:
	    	return new tHeatingCircleStatusSensor(SensorID);

	    default:
	    	return NULL;
	    }

		return NULL;
	}
};

class tSensorJsonFormatterFactory_app  : public tSensorJsonFormatterFactory {
public:
    tSensorJsonFormatterFactory_app() : tSensorJsonFormatterFactory() {}
protected:
    virtual tSensorJsonFormatter *appSpecificCreateJsonFormatter(uint8_t SensorType, uint8_t apiVersion)
    {
        switch (SensorType)
        {
		case SENSOR_TYPE_HEATING_CIRCLE_STATE:
            switch (apiVersion)
            {
            case 1:
                return new tHeatingCircleStatusSensorJsonFormat_api_1;
            }
            break;
       }
       return NULL;
    }
};


class tHttpServer_app : public tHttpServer {
public:
	tHttpServer_app() :
			tHttpServer() {}
protected:
	virtual tHttpServlet * ServletFactory(String *pRequestBuffer)
	{
		if (pRequestBuffer->startsWith("/heat")) return new tHeatingControlServlet();
		return NULL;
	}
};






/* instances of global objects */
tSensorFactory_app SensorFactory_app;
tSensorJsonFormatterFactory_app SensorJsonFormatterFactory;
tHttpServer_app HttpServer;



// create sensors and application specific to my home setup -  yes I know it should not be this way :)

tDS1820Sensor::DeviceAddress FloorTemperatureTempSensorSerial = { 0x28, 0x3C, 0x1F, 0x5F, 0xA1, 0x21, 0x01, 0xD9};
tDS1820Sensor::DeviceAddress RadiatorsTemperatureTempSensorSerial = { 0x28, 0x44, 0x9B, 0x80, 0xA1, 0x21, 0x01, 0xAF};
tDS1820Sensor::DeviceAddress HeatingSourceTempSensorSerial = { 0x28, 0x44, 0x9B, 0x80, 0xA1, 0x21, 0x01, 0xAF};
tDS1820Sensor::DeviceAddress HeatingStorageTempSensorSerial = { 0x28, 0x6D, 0xDB, 0x7E, 0xA1, 0x21, 0x01, 0x02};

tHeatingCircleControl *pFloorTemperatureValveControl;
tHeatingCircleControl *pRadiatorsTemperatureValveControl;
tImpulseSensor *pImpulseSensor = NULL;


class tHomeAutomationHeatingCentral: public tApplication {
public:
	tHomeAutomationHeatingCentral() : tApplication() {}

protected:

	virtual void AppSetupBefore() {
		// set default values for eeprom
		SetDefaultEEPromValues();
	}

	virtual void AppSetupAfter() {
		/* common sensors */
		tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_SYSTEM_STATUS, SENSOR_ID_SYSTEM_STATUS,F("SystemStatus"),1,NULL,0,50,true, 0);

		/* sensofactory for application Heating central*/
	    tDS1820Sensor::tConfig DS1820config;
	    tSimpleDigitalInputSensor::tConfig SimpleDigitalInputSensorConfig;
	    tPt100AnalogSensor::tConfig Pt100AnalogSensorConfig;
	    tHeatingCircleStatusSensor::tConfig HeatingCircleStatusSensorConfig;
	    tImpulseSensor::tConfig ImpulseSensorConfig;

	    DS1820config.Avg = 0;
	    DS1820config.Pin = 2;
	    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_DS1820, SENSOR_ID_1820_HEATING_TEMP,F("HeatingTemp"),1,&DS1820config,sizeof(DS1820config),50,true,0);   // 5sec

	    DS1820config.Avg = 0;
	    DS1820config.Pin = 3;
	    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_DS1820, SENSOR_ID_1820_AIR_HUW_TEMP,F("AirHuwTemp"), 1,&DS1820config,sizeof(DS1820config),50,true,0);   // 5sec

	    DS1820config.Avg = 0;
	    DS1820config.Pin = 7;
	    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_DS1820, SENSOR_ID_1820_OUTDOOR_TEMP, F("OutdoorTemp"),1,&DS1820config,sizeof(DS1820config),50,true,0);  // 5sec

	    pFloorTemperatureValveControl = new tHeatingCircleControl(
	          SENSOR_ID_1820_HEATING_TEMP,
	          FloorTemperatureTempSensorSerial,
	          HeatingSourceTempSensorSerial,
	          HeatingStorageTempSensorSerial,
	          OUT_ID_FLOOR_TEMP_HIGHER,
	          OUT_ID_FLOOR_TEMP_LOWER,
	          OUT_ID_FLOOR_PUMP,
	          2);
	    pRadiatorsTemperatureValveControl = new tHeatingCircleControl(
	          SENSOR_ID_1820_HEATING_TEMP,
	          RadiatorsTemperatureTempSensorSerial,
	          HeatingSourceTempSensorSerial,
	          HeatingStorageTempSensorSerial,
	          OUT_ID_RADIATOR_TEMP_HIGHER,
	          OUT_ID_RADIATOR_TEMP_LOWER,
	          OUT_ID_READIATORS_PUMP,
	          2);

	    pFloorTemperatureValveControl->setTargetTemp(28);
	    pFloorTemperatureValveControl->setTolerance(0.7);
	    pFloorTemperatureValveControl->setHisteresis(0.8);
	    pFloorTemperatureValveControl->setFastThold(1.5);

	    pRadiatorsTemperatureValveControl->setTargetTemp(31);
	    pRadiatorsTemperatureValveControl->setTolerance(0.7);
	    pRadiatorsTemperatureValveControl->setHisteresis(1);
	    pRadiatorsTemperatureValveControl->setFastThold(2);

	    SimpleDigitalInputSensorConfig.ActiveState = 0;
	    SimpleDigitalInputSensorConfig.Pin = A9;
	    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_DIGITAL_INPUT, SENSOR_ID_DIGITAL_WATER_HEATER_REQUEST, F("WaterHeaterRequest"),1,
	            &SimpleDigitalInputSensorConfig,sizeof(SimpleDigitalInputSensorConfig),10,true, 0);  // 1sec

	    SimpleDigitalInputSensorConfig.ActiveState = 0;
	    SimpleDigitalInputSensorConfig.Pin = A8;
	    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_DIGITAL_INPUT, SENSOR_ID_DIGITAL_RADIATORS_PUMP_CONTROL, F("PumpControl"),1,
	            &SimpleDigitalInputSensorConfig,sizeof(SimpleDigitalInputSensorConfig),10,true, 0);  // 1sec

	    SimpleDigitalInputSensorConfig.ActiveState = 0;
	    SimpleDigitalInputSensorConfig.Pin = A7;
	    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_DIGITAL_INPUT, SENSOR_ID_DIGITAL_AUX, F("DigitalInputAux"),1,
	            &SimpleDigitalInputSensorConfig,sizeof(SimpleDigitalInputSensorConfig),10,true,0);  // 1sec

	    ImpulseSensorConfig.Pin = 21;
	    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_IMPULSE, SENSOR_ID_IMPULSE_HEATPUMP, F("HeatPumpEnergy"),1,
	    		&ImpulseSensorConfig,sizeof(ImpulseSensorConfig),50,true, 0);


	    Pt100AnalogSensorConfig.Pin = A14;
	    Pt100AnalogSensorConfig.Correction = 8;
	    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_PT100_ANALOG, SENSOR_ID_PT100_HOTAIR, F("HotAir"),1,
	    		&Pt100AnalogSensorConfig,sizeof(Pt100AnalogSensorConfig),50,true, 0);

	    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_OUTPUT_STATES, SENSOR_ID_OUTPUT_STATES, F("OutStates"),1,
	    		NULL,0,50,true, 0);


	    HeatingCircleStatusSensorConfig.pHeatingControl = pRadiatorsTemperatureValveControl;
	    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_HEATING_CIRCLE_STATE, SENSOR_ID_RADIATORS_HEATING_STATE,
	    									   F("RadiatorsState"), 1, &HeatingCircleStatusSensorConfig, sizeof(HeatingCircleStatusSensorConfig), 50, true, 0);

	    HeatingCircleStatusSensorConfig.pHeatingControl = pFloorTemperatureValveControl;
	    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_HEATING_CIRCLE_STATE, SENSOR_ID_FLOOR_HEATING_STATE,
	    									   F("FloorState"), 1, &HeatingCircleStatusSensorConfig, sizeof(HeatingCircleStatusSensorConfig), 50, true, 0);
	}
};

tHomeAutomationHeatingCentral HomeAutomationHeatingCentral;


#endif
