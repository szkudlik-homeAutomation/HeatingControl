@echo off
setlocal

if "%1"==""  goto args_count_wrong
if "%2"==""  goto args_count_wrong

:: split 32bit word to bytes
set /a "byte0=%2 & 0xFF"
set /a "byte1=(%2 >> 8) & 0xFF"
set /a "byte2=(%2 >> 16) & 0xFF"
set /a "byte3=(%2 >> 24) & 0xFF"

::disable eeprom safe
..\tools\avrdude.exe -pm328p -cusbasp -Ulfuse:w:0xff:m -Uhfuse:w:0xdf:m -Uefuse:w:0x04:m -B 125kHz

::erase chip and upload FW
..\tools\avrdude.exe -pm328p -cusbasp -e -B 125kHz

::set chip ID and enabled sensors in flash
..\tools\avrdude.exe -pm328p -cusbasp -Ueeprom:w:%1,%byte0%,%byte1%,%byte2%,%byte3%:m -B 125kHz

::enable eeprom safe
..\tools\avrdude.exe -pm328p -cusbasp -U lfuse:w:0xff:m -U hfuse:w:0xd7:m -U efuse:w:0x04:m -B 125kHz

::upload flash
..\tools\avrdude.exe -pm328p -cusbasp -Uflash:w:build\Node\HeatingControl.ino.hex:i -B 125kHz

goto end

:args_count_wrong
echo usage: format.bat id_of_node(0x for hex) bitmap_of_active_sensors
echo where sensors are
echo 01 - system status
echo 02 - SHT3 temperature and humidity
echo 04 - TGS_2603 odour sensors


:end

endlocal