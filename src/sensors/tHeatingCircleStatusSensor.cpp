/*
 * tHeatingCircleStatusSensor.cpp
 *
 *  Created on: 27 lis 2022
 *      Author: szkud
 */

#include "../../global.h"
#if CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR

#include "tHeatingCircleStatusSensor.h"
#include "../tHeatingCircleControl.h"


uint8_t tHeatingCircleStatusSensor::doSetConfig()
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

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t tHeatingCircleStatusSensorDesc::doFormatJSON(Stream *pStream)
{
   if (dataBlobSize != sizeof(tHeatingCircleStatusSensor::tResult))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tHeatingCircleStatusSensor::tResult *pResult =(tHeatingCircleStatusSensor::tResult *) pDataCache;
   pStream->print(F("\"IsWorking\":"));
   pStream->print(pResult->isWorking);

   pStream->print(F(", \"TargetTemp\":"));
   pStream->print(pResult->TargetTemp);
   pStream->print(F(","));

   return STATUS_SUCCESS;
}
#endif //CONFIG_SENSORS_JSON_OUTPUT

#endif // CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR
