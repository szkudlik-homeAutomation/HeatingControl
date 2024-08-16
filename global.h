#pragma once

#if APP_HeatingCentral
#include "generated/HeatingCentral.config.h"
#else
#error "Unknown app."
#endif

#include "src/Common_code/commonGlobals.h"

#include "GlobalDefs/version.h"
#include "GlobalDefs/Eeprom.h"
#include "GlobalDefs/SensorId.h"
