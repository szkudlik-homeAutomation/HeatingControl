/*
 * tHeatingCircleStatusSensor.h
 *
 *  Created on: 27 lis 2022
 *      Author: szkud
 */

#pragma once

#include "../../global.h"

#define SENSOR_TYPE_HEATING_CIRCLE_STATE 128

#include "../Common_code/sensors/tSensor.h"
#include "../Common_code/sensors/tSensorCache.h"
#include "../Common_code/sensors/tSensorJsonOutput.h"

#if CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR_JSON_OUTPUT

class tHeatingCircleStatusSensorJsonFormat_api_1 : public tSensorJsonFormatter {
public:
    tHeatingCircleStatusSensorJsonFormat_api_1() : tSensorJsonFormatter() {}
protected:
    virtual uint8_t FormatJSON(Stream *pStream, tSensorCache *cache);
};

#endif //CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR_JSON_OUTPUT

#if CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR || CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR_JSON_OUTPUT

class tHeatingCircleControl;
class tHeatingCircleStatusSensorTypes
{
public:

   typedef struct
   {
      uint8_t isWorking;
      float TargetTemp;
   } tResult_api_v1;

   typedef struct
   {
      tHeatingCircleControl *pHeatingControl;
   } tConfig_api_v1;

};

#endif

#if CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR
class tHeatingCircleStatusSensor : public tSensor, tHeatingCircleStatusSensorTypes {
public:
   static const uint8_t API_VERSION = 1;
   typedef tConfig_api_v1 tConfig;
   typedef tResult_api_v1 tResult;

   tConfig Config;

   tHeatingCircleStatusSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_HEATING_CIRCLE_STATE, sensorID, API_VERSION, sizeof(tConfig), &Config) {}

protected:
   virtual void doTriggerMeasurement();
   virtual uint8_t onSetConfig();
private:
   tResult mResult;
};

#endif //CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR
