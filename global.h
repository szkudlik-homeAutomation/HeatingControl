/*
 * global.h
 *
 * a common file that MUST be included in every .h and .c**
 *
 *  Created on: 6 pa� 2022
 *      Author: szkud
 */

#pragma once


// #define CONFIG_TLE8457_serial_lib

//#ifdef __AVR_ATmega2560__
//#define CONFIG_CENTRAL_NODE 1
//#elif __AVR_ATmega328P__
//#define CONFIG_CENTRAL_NODE 0
//#else
//#error unknown board
//#endif

#define CONFIG_CENTRAL_NODE 1

#if CONFIG_CENTRAL_NODE

#define CONFIG_LOGGER 1

#define DEBUG_3_ENABLE
#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_EVENT serialEvent

#define CONFIG_HEATING_CIRCLE_CONTROL 1
#define CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR 1
#define CONFIG_SENSORS 1
#define CONFIG_SENSOR_HUB 1
#define CONFIG_SENSORS_JSON_OUTPUT 1
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

#endif //CONFIG_CENTRAL_NODE


#include <Arduino.h>
#include "GlobalDefs/version.h"
#include "GlobalDefs/Eeprom.h"
#include "GlobalDefs/outputPins.h"
#include "src/Common_code/debug.h"
#include "src/Common_code/helpers.h"
#include "src/Common_code/status.h"
