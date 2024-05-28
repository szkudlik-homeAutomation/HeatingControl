arduino-cli.exe compile -b arduino:avr:mega:cpu=atmega2560 HeatingControl.ino -v -e  --build-property "build.extra_flags=\"-DAPP_HeatingCentral=1\""
rmdir build\HeatingCentral /q /S
rename build\arduino.avr.mega HeatingCentral

