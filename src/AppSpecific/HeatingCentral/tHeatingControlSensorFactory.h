/*
 * tHeatingControlSensorFactory.h
 *
 *  Created on: Feb 6, 2024
 *      Author: mszkudli
 */

#pragma once

#include "../../../global.h"
#include "../../Common_code/sensors/tSensorFactory.h"
#include "tHeatingCircleStatusSensor.h"

class tHeatingConrolSensorFactory : public tSensorFactory
{
public:
    tHeatingConrolSensorFactory() : tSensorFactory() {}
protected:
   virtual doFormatJSON appSpecificGetSJONFrormatFunction(uint8_t SensorType, uint8_t apiVersion)
   {
        switch (SensorType)
        {
#if CONFIG_HEATING_CIRCLE_CONTROL_SERVLET
        case SENSOR_TYPE_HEATING_CIRCLE_STATE:
              switch (apiVersion)
              {
              case 1: return HeatingCircleStatusSensorJsonFormat_api_1;
              }
              break;
#endif // CONFIG_HEATING_CIRCLE_CONTROL_SERVLET
         default:
              return NULL;
        }

        return NULL;
   }

   virtual tSensor *appSpecificCreateSensor(uint8_t SensorType, uint8_t SensorID)
   {
       tSensor *newSensor= NULL;
       switch (SensorType)
       {
#if CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR
           case SENSOR_TYPE_HEATING_CIRCLE_STATE:
               newSensor = new tHeatingCircleStatusSensor(SensorID);
               break;
#endif /* CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR */
           default:
               newSensor = NULL;
       }

       return newSensor;
   }
};
