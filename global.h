/*
 * global.h
 *
 * a common file that MUST be included in every .h and .c**
 *
 *  Created on: 6 pa� 2022
 *      Author: szkud
 */

#pragma once


#define REMOTE_SENSORS_TEST 0

#define COMM_SERIAL Serial1
#define COMM_SERIAL_EVENT serialEvent1

#define CONFIG_LOGGER 1

#define DEBUG_3_ENABLE
#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_EVENT serialEvent

#define CONFIG_OUTPUT_PROCESS 1

#define CONFIG_SENSORS 1
#define CONFIG_EEPROM_SENSORS 1
#define CONFIG_SENSOR_HUB 1
#define CONFIG_SENSOR_HUB_GENERATE_EVENTS 0
#define CONFIG_SENSOR_GENERATE_EVENTS 1
#define CONFIG_SENSORS_JSON_OUTPUT 1
#define CONFIG_SENSORS_OVER_SERIAL_COMM 1
#define CONFIG_SENSOR_GENERATE_SERIAL_EVENTS 1
#define CONFIG_TELNET_COMMANDS_SENSORS 1

#define CONFIG_DS1820_SENSOR 1
#define CONFIG_OUTPUT_STATE_SENSOR 1
#define CONFIG_IMPULSE_SENSOR 1
#define CONFIG_PT100_ANALOG_SENSOR 1
#define CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR 1
#define CONFIG_SYSTEM_STATUS_SENSOR 1
#define CONFIG_NETWORK 1
#define CONFIG_HTTP_SERVER 1
#define CONFIG_HTTP_SERVLET_OUTCONTROL_JS 1
#define CONFIG_SENSOR_STATE_SERVLET 1
#define CONFIG_OUTPUT_CONTROL_SERVLET 1
#define CONFIG_TELNET_SERVER 1

#define CONFIG_WATCHDOG 1
// restart if no connection for 5 minutes
#define TCP_WATCHDOG_TIMEOUT 300

#define CONFIG_WORKER_PROCESS 1
#define CONFIG_NODE_SCAN_TASK 1

#define CONFIG_TLE8457_COMM_LIB 1

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
