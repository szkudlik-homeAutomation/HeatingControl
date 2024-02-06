/*
 * tHeatingControlSensorFactory.h
 *
 *  Created on: Feb 6, 2024
 *      Author: mszkudli
 */

#pragma once

#include "../global.h"
#include "Common_code/sensors/tSensorFactory.h"

class tHeatingConrolSensorFactory : public tSensorFactory
{
public:
    tHeatingConrolSensorFactory() : tSensorFactory() {}
protected:
   virtual doFormatJSON appSpecificGetSJONFrormatFunction(uint8_t SensorType, uint8_t apiVersion)
   {
        switch (SensorType)
        {
           case SENSOR_TYPE_HEATING_CIRCLE_STATE:
              switch (apiVersion)
              {
              case 1:
                  return HeatingCircleStatusSensorJsonFormat_api_1;
              }
              break;
        }

        return NULL;
   }

   virtual tSensor *appSpecificCreateSensor(uint8_t SensorType, uint8_t SensorID)
   {
       tSensor *newSensor= NULL;
       switch (SensorType)
       {
           case SENSOR_TYPE_HEATING_CIRCLE_STATE:
               newSensor = new tHeatingCircleStatusSensor(SensorID);
               break;
       }

       return newSensor;
   }
};
