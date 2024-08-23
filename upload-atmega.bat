set PORT=com13
arduino-cli.exe upload -b arduino:avr:mega:cpu=atmega2560 -v -p %PORT%
arduino-cli.exe monitor -c baudrate=115200 -p %PORT%
