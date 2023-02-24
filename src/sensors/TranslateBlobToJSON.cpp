#include "../../global.h"
#if CONFIG_SENSORS_JSON_OUTPUT
#include "../Common_code/sensors/tSensor.h"


#if CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR
#include "tHeatingCircleStatusSensor.h"
#endif


uint8_t appTranslateBlobToJSON(uint8_t SensorType, uint8_t dataBlobSize, void *pDataCache, Stream *pStream)
{
	   uint8_t Result;

	   switch (SensorType)
	   {
		#if CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR
			  case SENSOR_TYPE_HEATING_CIRCLE_STATE:
				 Result = tHeatingCircleStatusSensor::TranslateBlobToJSON(dataBlobSize,pDataCache,pStream);
				 break;
		#endif
			  default:
				  Result = STATUS_UNKNOWN_SENSOR_TYPE;
				  break;
	   }
	return Result;
}

#endif
