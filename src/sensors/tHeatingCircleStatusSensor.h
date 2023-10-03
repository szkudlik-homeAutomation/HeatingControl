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

class tHeatingCircleControl;
class tHeatingCircleStatusSensor : public tSensor {
public:
   static const uint8_t API_VERSION = 1;
   tHeatingCircleStatusSensor() : tSensor(SENSOR_TYPE_HEATING_CIRCLE_STATE, API_VERSION) {}

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

#if CONFIG_SENSORS_JSON_OUTPUT
   static uint8_t TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream);
#endif //CONFIG_SENSORS_JSON_OUTPUT

protected:
   virtual void doTriggerMeasurement();
   virtual uint8_t doSetConfig();
private:
   tResult mResult;
};

#endif //CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR
