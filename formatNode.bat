@echo off
setlocal

if "%4"==""  goto args_count_wrong

:: split 32bit word to bytes
set /a "byte0=%2 & 0xFF"
set /a "byte1=(%2 >> 8) & 0xFF"
set /a "byte2=(%2 >> 16) & 0xFF"
set /a "byte3=(%2 >> 24) & 0xFF"

::disable eeprom safe
..\tools\avrdude.exe -pm328p -cusbasp -Ulfuse:w:0xff:m -Uhfuse:w:0xdf:m -Uefuse:w:0x04:m -B 125kHz

::erase chip and upload FW
..\tools\avrdude.exe -pm328p -cusbasp -e -B 125kHz

::set chip ID and enabled sensors in flash: 4 bytes sensor mask, 8 bytes GPIO out enable, 8 bit GPIO out polarity
..\tools\avrdude.exe -pm328p -cusbasp -Ueeprom:w:%1,%byte0%,%byte1%,%byte2%,%byte3%,%3,%4:m -B 125kHz

::enable eeprom safe
..\tools\avrdude.exe -pm328p -cusbasp -U lfuse:w:0xff:m -U hfuse:w:0xd7:m -U efuse:w:0x04:m -B 125kHz

::upload flash
..\tools\avrdude.exe -pm328p -cusbasp -Uflash:w:build\Node\HeatingControl.ino.hex:i -B 125kHz

goto end

:args_count_wrong
echo usage: format.bat id_of_node(0x for hex) bitmap_of_active_sensors bitmap_of_active_GPIOs polarity_of_GPIOs
echo where sensors are
echo -  01 - system status
echo -  02 - SHT3 temperature and humidity
echo -  04 - TGS_2603 odour sensors
echo -  08 - DS1820 sensors, GPIO6
echo bitmap_of_active_GPIOs a bitmap 0-5 of GPIO outputs to be externally controlled. Note that some sensors may use some of GPIOs
echo polarity_of_GPIOs bitmap of GPIOs polarity. 0 means active low

:end

endlocal