/*
 * tNodeOutputProcess.cpp
 *
 *  Created on: 14 lis 2025
 *      Author: szkud
 */

#include "tNodeOutputProcess.h"
#include "../GlobalDefs/Eeprom.h"

#if CONFIG_EEPROM_ENABLED_GPIO

tNodeOutputProcess NodeOutputProcess;

#define is_gpio_enabled(_Bitmap, _gpio) ((_Bitmap) & (1 << (_gpio)))


void tNodeOutputProcess::setup()
{
	// read output enable bitmap from eeprom
	uint8_t OutputEnableBitmap = 0;
	EEPROM.get(EEPROM_GPIO_ENABLE_BITMAP_OFFSET, OutputEnableBitmap);

	uint8_t OutputPolarity = 0;
	// read output polarity bitmap from eeprom
	EEPROM.get(EEPROM_GPIO_POLARITY_BITMAP_OFFSET,OutputPolarity);

	#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 0
	   Output[0].SetPin(
	     ((OutputEnableBitmap &  (1 << 0)) != 0) ? CONFIG_OUTPUT_PROCESS_PIN0 : tOutput::PIN_NOT_ASSIGNED,
		 ((OutputPolarity &  (1 << 0)) == 0) ? 0 : 1);
	#endif
	#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 1
	   Output[1].SetPin(
	     ((OutputEnableBitmap &  (1 << 1)) != 0) ? CONFIG_OUTPUT_PROCESS_PIN1 : tOutput::PIN_NOT_ASSIGNED,
		 ((OutputPolarity &  (1 << 1)) == 0) ? 0 : 1);
	#endif
	#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 2
	   Output[2].SetPin(
	     ((OutputEnableBitmap &  (1 << 2)) != 0) ? CONFIG_OUTPUT_PROCESS_PIN2 : tOutput::PIN_NOT_ASSIGNED,
		 ((OutputPolarity &  (1 << 2)) == 0) ? 0 : 1);
	#endif
	#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 3
	   Output[3].SetPin(
	     ((OutputEnableBitmap &  (1 << 3)) != 0) ? CONFIG_OUTPUT_PROCESS_PIN3 : tOutput::PIN_NOT_ASSIGNED,
		 ((OutputPolarity &  (1 << 3)) == 0) ? 0 : 1);
	#endif
	#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 4
	   Output[4].SetPin(
	     ((OutputEnableBitmap &  (1 << 4)) != 0) ? CONFIG_OUTPUT_PROCESS_PIN4 : tOutput::PIN_NOT_ASSIGNED,
		 ((OutputPolarity &  (1 << 4)) == 0) ? 0 : 1);
	#endif
	#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 5
	   Output[5].SetPin(
	     ((OutputEnableBitmap &  (1 << 5)) != 0) ? CONFIG_OUTPUT_PROCESS_PIN5 : tOutput::PIN_NOT_ASSIGNED,
		 ((OutputPolarity &  (1 << 5)) == 0) ? 0 : 1);
	#endif
	#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 6
	   Output[6].SetPin(
	     ((OutputEnableBitmap &  (1 << 6)) != 0) ? CONFIG_OUTPUT_PROCESS_PIN6 : tOutput::PIN_NOT_ASSIGNED,
		 ((OutputPolarity &  (1 << 6)) == 0) ? 0 : 1);
	#endif
	#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 7
	   Output[7].SetPin(
	     ((OutputEnableBitmap &  (1 << 7)) != 0) ? CONFIG_OUTPUT_PROCESS_PIN7 : tOutput::PIN_NOT_ASSIGNED,
		 ((OutputPolarity &  (1 << 7)) == 0) ? 0 : 1);
	#endif

}


#endif CONFIG_EEPROM_ENABLED_GPIO
