/*
 * HeatingCentralGlobalObjects.cpp
 *
 *  Created on: May 14, 2024
 *      Author: mszkudli
 */

#include "../../../global.h"

#include "../../Common_code/sensors/tSensor.h"
#include "../../Common_code/sensors/tSensorFactory.h"
#include "../../Common_code/sensors/tDS1820Sensor.h"
#include "../../Common_code/sensors/tImpulseSensor.h"
#include "../../Common_code/sensors/tPt100AnalogSensor.h"
#include "../../Common_code/sensors/tSimpleDigitalInputSensor.h"

#include "../../tHeatingControlSensorFactory.h"




#if APP_generalTest


void AppSetup()
{
    tSensorFactory::Instance->CreateSensor(SENSOR_TYPE_SYSTEM_STATUS, SENSOR_ID_SYSTEM_STATUS,"SystemStatus",1,NULL,0,50,true);
}

#endif  APP_generalTest
