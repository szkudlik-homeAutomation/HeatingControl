/*
 * HeatingCentralGlobalObjects.cpp
 *
 *  Created on: May 14, 2024
 *      Author: mszkudli
 */

#include "../../../global.h"

#include "../../Common_code/sensors/tSensor.h"
#include "../../Common_code/sensors/tSensorFactory.h"
#include "../../Common_code/sensors/tSensorHub.h"
#include "../../Common_code/sensors/tDS1820Sensor.h"
#include "../../Common_code/sensors/tImpulseSensor.h"
#include "../../Common_code/sensors/tPt100AnalogSensor.h"
#include "../../Common_code/sensors/tSimpleDigitalInputSensor.h"

#include "tHeatingCircleControl.h"
#include "tHeatingCircleStatusSensor.h"



#if APP_HeatingCentral

tPt100SensorLogger Pt100SensorLogger(SENSOR_ID_PT100_HOTAIR);
tDS1820SensorLogger DS1820SensorLogger(0);  // all DS1820s
tImpulseSensorLogger ImpulseSensorLogger(SENSOR_ID_IMPULSE_HEATPUMP);

tDS1820Sensor::DeviceAddress FloorTemperatureTempSensorSerial = { 0x28, 0x3C, 0x1F, 0x5F, 0xA1, 0x21, 0x01, 0xD9};
tDS1820Sensor::DeviceAddress RadiatorsTemperatureTempSensorSerial = { 0x28, 0x44, 0x9B, 0x80, 0xA1, 0x21, 0x01, 0xAF};
tDS1820Sensor::DeviceAddress HeatingSourceTempSensorSerial = { 0x28, 0x44, 0x9B, 0x80, 0xA1, 0x21, 0x01, 0xAF};
tDS1820Sensor::DeviceAddress HeatingStorageTempSensorSerial = { 0x28, 0x6D, 0xDB, 0x7E, 0xA1, 0x21, 0x01, 0x02};

tHeatingCircleControl *pFloorTemperatureValveControl;
tHeatingCircleControl *pRadiatorsTemperatureValveControl;
tImpulseSensor *pImpulseSensor = NULL;

void AppSetup()
{
    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_SYSTEM_STATUS, SENSOR_ID_SYSTEM_STATUS,"SystemStatus",1,NULL,0,50,true);

    tDS1820Sensor::tConfig DS1820config;
    tSimpleDigitalInputSensor::tConfig SimpleDigitalInputSensorConfig;
    tPt100AnalogSensor::tConfig Pt100AnalogSensorConfig;
    tHeatingCircleStatusSensor::tConfig HeatingCircleStatusSensorConfig;
    tImpulseSensor::tConfig ImpulseSensorConfig;

    DS1820config.Avg = 0;
    DS1820config.Pin = 2;
    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_DS1820, SENSOR_ID_1820_HEATING_TEMP,"HeatingTemp",1,&DS1820config,sizeof(DS1820config),50,true);   // 5sec

    DS1820config.Avg = 0;
    DS1820config.Pin = 3;
    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_DS1820, SENSOR_ID_1820_AIR_HUW_TEMP,"AirHuwTemp", 1,&DS1820config,sizeof(DS1820config),50,true);   // 5sec

    DS1820config.Avg = 0;
    DS1820config.Pin = 7;
    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_DS1820, SENSOR_ID_1820_OUTDOOR_TEMP, "OutdoorTemp",1,&DS1820config,sizeof(DS1820config),50,true);  // 5sec

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
    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_DIGITAL_INPUT, SENSOR_ID_DIGITAL_WATER_HEATER_REQUEST, "WaterHeaterRequest",1,
            &SimpleDigitalInputSensorConfig,sizeof(SimpleDigitalInputSensorConfig),10,true);  // 1sec

    SimpleDigitalInputSensorConfig.ActiveState = 0;
    SimpleDigitalInputSensorConfig.Pin = A8;
    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_DIGITAL_INPUT, SENSOR_ID_DIGITAL_RADIATORS_PUMP_CONTROL, "PumpControl",1,
            &SimpleDigitalInputSensorConfig,sizeof(SimpleDigitalInputSensorConfig),10,true);  // 1sec

    SimpleDigitalInputSensorConfig.ActiveState = 0;
    SimpleDigitalInputSensorConfig.Pin = A7;
    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_DIGITAL_INPUT, SENSOR_ID_DIGITAL_AUX, "DigitalInputAux",1,
            &SimpleDigitalInputSensorConfig,sizeof(SimpleDigitalInputSensorConfig),10,true);  // 1sec

    ImpulseSensorConfig.Pin = 21;
    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_IMPULSE, SENSOR_ID_IMPULSE_HEATPUMP, "HeatPumpEnergy",1,&ImpulseSensorConfig,sizeof(ImpulseSensorConfig),50,true);


    Pt100AnalogSensorConfig.Pin = A14;
    Pt100AnalogSensorConfig.Correction = 8;
    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_PT100_ANALOG, SENSOR_ID_PT100_HOTAIR, "HotAir",1,&Pt100AnalogSensorConfig,sizeof(Pt100AnalogSensorConfig),50,true);

    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_OUTPUT_STATES, SENSOR_ID_OUTPUT_STATES, "OutStates",1,NULL,0,50,true);


    HeatingCircleStatusSensorConfig.pHeatingControl = pRadiatorsTemperatureValveControl;
    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_HEATING_CIRCLE_STATE, SENSOR_ID_RADIATORS_HEATING_STATE,
    									   "RadiatorsState", 1, &HeatingCircleStatusSensorConfig, sizeof(HeatingCircleStatusSensorConfig), 50, true);

    HeatingCircleStatusSensorConfig.pHeatingControl = pFloorTemperatureValveControl;
    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_HEATING_CIRCLE_STATE, SENSOR_ID_FLOOR_HEATING_STATE,
    									   "FloorState", 1, &HeatingCircleStatusSensorConfig, sizeof(HeatingCircleStatusSensorConfig), 50, true);
}

#endif APP_HeatingCentral
