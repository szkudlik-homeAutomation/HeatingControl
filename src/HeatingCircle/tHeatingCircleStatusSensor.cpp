/*
 * tHeatingCircleStatusSensor.cpp
 *
 *  Created on: 27 lis 2022
 *      Author: szkud
 */

#include "../../global.h"
#if CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR

#include "tHeatingCircleStatusSensor.h"
#include "tHeatingCircleControl.h"


uint8_t tHeatingCircleStatusSensor::onSetConfig()
{
   mCurrentMeasurementBlob = (void*) &mResult;
   mMeasurementBlobSize = sizeof(mResult);
   return STATUS_SUCCESS;
}


void tHeatingCircleStatusSensor::doTriggerMeasurement()
{
   mResult.TargetTemp = Config.pHeatingControl->getTargetTemp();
   mResult.isWorking = Config.pHeatingControl->isWorking();
   onMeasurementCompleted(true);
}
#endif // CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR

#if CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR_JSON_OUTPUT
uint8_t HeatingCircleStatusSensorJsonFormat_api_1(Stream *pStream, tSensorCache *cache)
{
   if (cache->getDataBlobSize() != sizeof(tHeatingCircleStatusSensor::tResult))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tHeatingCircleStatusSensor::tResult *pResult =(tHeatingCircleStatusSensor::tResult *) cache->getData();
   pStream->print(F("\"IsWorking\":"));
   pStream->print(pResult->isWorking);

   pStream->print(F(", \"TargetTemp\":"));
   pStream->print(pResult->TargetTemp);

   return STATUS_SUCCESS;
}
#endif //CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR_JSON_OUTPUT

