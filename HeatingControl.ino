

#include "global.h"
#include "src/Common_code/tApplication.h"

class tHomeAutomation: public tApplication {
public:
	tHomeAutomation() : tApplication() {}

protected:

	virtual void AppSetupBefore() {
		// set default values for eeprom
		SetDefaultEEPromValues();
	}
};

tHomeAutomation HomeAutomation;
