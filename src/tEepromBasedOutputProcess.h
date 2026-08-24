#pragma once

#include "../global.h"
#include "Common_code/tOutputProcess.h"

#if CONFIG_EEPROM_ENABLED_GPIO_OUTPUTS

class tEepromBasedOutputProcess: public tOutputProcess {
public:
	tEepromBasedOutputProcess() {};

	virtual void setup() final;
};

#endif // CONFIG_EEPROM_ENABLED_GPIO_OUTPUTS
