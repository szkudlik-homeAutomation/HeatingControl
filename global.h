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

#ifdef __AVR_ATmega2560__
#define CONTROLLER
#elif __AVR_ATmega328P__
#else
#error unknown board
#endif



#include <Arduino.h>
#include "GlobalDefs/version.h"
#include "GlobalDefs/Eeprom.h"
#include "GlobalDefs/outputPins.h"
#include "src/Common_code/debug.h"
#include "src/Common_code/helpers.h"

