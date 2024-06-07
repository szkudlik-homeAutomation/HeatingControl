/*
 * global.h
 *
 * a common file that MUST be included in every .h and .c**
 *
 *  Created on: 6 pa� 2022
 *      Author: szkud
 */

#pragma once


#define CONFIG_WATCHDOG 1
// restart if no connection for 5 minutes
#define TCP_WATCHDOG_TIMEOUT 300

#define CONFIG_TLE8457_COMM_LIB 1

#define CONFIG_SENSORS 1
#define CONFIG_SYSTEM_STATUS_SENSOR 1
#define CONFIG_SENSORS_OVER_SERIAL_COMM 1

#define CONFIG_EEPROM_SENSORS 1

#if APP_HeatingCentral
#include "appDefs\HeatingCental.h"
#elif APP_generalTest
#include "appDefs\generalTest.h"
#elif APP_node
#include "appDefs\node.h"
#else
#error no app defined
#endif

#include <Arduino.h>
#include "src/Common_code/helpers.h"
#include "GlobalDefs/version.h"
#include "GlobalDefs/Eeprom.h"
#include "GlobalDefs/CommDefs.h"
#include "GlobalDefs/SensorId.h"
#include "src/Common_code/debug.h"
#include "src/Common_code/status.h"
