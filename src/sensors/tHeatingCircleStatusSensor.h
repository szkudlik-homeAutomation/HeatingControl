/*
 * tHeatingCircleStatusSensor.h
 *
 *  Created on: 27 lis 2022
 *      Author: szkud
 */

#pragma once

#include "../../global.h"
#if CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR

#include "../Common_code/sensors/tSensor.h"
#include "../Common_code/sensors/tSensorDesc.h"

class tHeatingCircleStatusSensorDesc : public tSensorDesc
{
public:
    tHeatingCircleStatusSensorDesc(uint8_t aSensorID, char * apSensorName) :
        tSensorDesc(SENSOR_TYPE_HEATING_CIRCLE_STATE, aSensorID, apSensorName) {}

protected:
#if CONFIG_SENSORS_JSON_OUTPUT
   /* sensor specific JSON formatter */
    virtual uint8_t doFormatJSON(Stream *pStream);
#endif // CONFIG_SENSORS_JSON_OUTPUT
};

class tHeatingCircleControl;
class tHeatingCircleStatusSensor : public tSensor {
public:
   static const uint8_t API_VERSION = 1;
   tHeatingCircleStatusSensor() : tSensor(SENSOR_TYPE_HEATING_CIRCLE_STATE, API_VERSION, sizeof(tConfig), &Config) {}

   typedef struct
   {
      uint8_t isWorking;
      float TargetTemp;
   } tResult;

   typedef struct
   {
      tHeatingCircleControl *pHeatingControl;
   } tConfig;

   tConfig Config;

protected:
   virtual void doTriggerMeasurement();
   virtual uint8_t doSetConfig();
private:
   tResult mResult;
};

#endif //CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR
