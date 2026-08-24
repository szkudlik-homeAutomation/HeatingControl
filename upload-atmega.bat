set PORT=com6
arduino-cli.exe upload -b arduino:avr:mega:cpu=atmega2560 -v -p %PORT% -i build\HeatingCentral\HeatingControl.ino.with_bootloader.bin
arduino-cli.exe monitor -c baudrate=115200 -p %PORT%
