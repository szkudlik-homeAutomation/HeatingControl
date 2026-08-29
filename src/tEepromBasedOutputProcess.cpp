/*
 * tEepromBasedOutputProcess.cpp
 *
 *  Created on: 14 lis 2025
 *      Author: szkud
 */

#include "tEepromBasedOutputProcess.h"
#include "../GlobalDefs/Eeprom.h"

#if CONFIG_EEPROM_ENABLED_GPIO_OUTPUTS

tEepromBasedOutputProcess NodeOutputProcess;

#define is_gpio_enabled(_Bitmap, _gpio) ((_Bitmap) & (1 << (_gpio)))


tEepromBasedOutputProcess::tEepromBasedOutputProcess()
{
	// read output enable bitmap from eeprom
	// as number of outputs must be set before calling setup of tOutputStateSensor, we need to read the bitmap here
	
	mNumOfOutputs = 0;
	uint8_t OutputEnableBitmap = 0;
	EEPROM.get(EEPROM_GPIO_ENABLE_BITMAP_OFFSET, OutputEnableBitmap);

	uint8_t OutputPolarity = 0;
	// read output polarity bitmap from eeprom
	EEPROM.get(EEPROM_GPIO_POLARITY_BITMAP_OFFSET,OutputPolarity);

	for (uint8_t i = 0; i < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS; i++)
	{
		if (((OutputEnableBitmap &  (1 << i)) != 0))
		{
			uint8_t pin;
			switch (i)
			{
				#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 0
					case 0: pin = CONFIG_OUTPUT_PROCESS_PIN0; break;
				#endif
				#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 1
					case 1: pin = CONFIG_OUTPUT_PROCESS_PIN1; break;
				#endif
				#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 2
					case 2: pin = CONFIG_OUTPUT_PROCESS_PIN2; break;
				#endif
				#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 3
					case 3: pin = CONFIG_OUTPUT_PROCESS_PIN3; break;
				#endif
				#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 4
					case 4: pin = CONFIG_OUTPUT_PROCESS_PIN4; break;
				#endif
				#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 5
					case 5: pin = CONFIG_OUTPUT_PROCESS_PIN5; break;
				#endif
				#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 6
					case 6: pin = CONFIG_OUTPUT_PROCESS_PIN6; break;
				#endif
				#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 7
					case 7: pin = CONFIG_OUTPUT_PROCESS_PIN7; break;
				#endif
				default:
					pin = tOutput::PIN_NOT_ASSIGNED;
					break;
			}
			Output[mNumOfOutputs].SetPin(pin, (OutputPolarity &  (1 << i)) == 0 ? 0 : 1);

			mNumOfOutputs++;
		}
	}
}


#endif // CONFIG_EEPROM_ENABLED_GPIO_OUTPUTS
