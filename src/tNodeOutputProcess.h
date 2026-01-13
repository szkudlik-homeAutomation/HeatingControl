#pragma once

#include "../global.h"
#include "Common_code/tOutputProcess.h"

#if CONFIG_EEPROM_ENABLED_GPIO

class tNodeOutputProcess: public tOutputProcess {
public:
	tNodeOutputProcess() {};

	virtual void setup() final;
};

#endif // CONFIG_EEPROM_ENABLED_GPIO
