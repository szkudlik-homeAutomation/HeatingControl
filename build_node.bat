arduino-cli.exe compile -b arduino:avr:nano HeatingControl.ino -v -e  --build-property "build.extra_flags=\"-DAPP_node=1\""
rmdir build\node /q /S
rename build\arduino.avr.nano node
