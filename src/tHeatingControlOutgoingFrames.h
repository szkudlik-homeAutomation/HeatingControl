/*
 * tHeatingControlOutgoingFrames.h
 *
 *  Created on: Jan 17, 2023
 *      Author: mszkudli
 */

#pragma once

#include "../global.h"
#if CONFIG_TLE8457_COMM_LIB
#include "Common_code/TLE8457_serial/tOutgoingFrames.h"

class tHeatingControlOutgoingFrames : public tOutgoingFrames {

};
#endif //CONFIG_TLE8457_COMM_LIB
