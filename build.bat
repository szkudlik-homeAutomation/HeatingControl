@echo off
set APP_NAME=%1

:: merge main config with app overlay

python src\Common_code\scripts\merge_config.py GlobalDefs/common.config appDefs\%APP_NAME%.config generated\%APP_NAME%.config

:: generate config
python src\Common_code\scripts\kconfg.py generated\%APP_NAME%.config generated\%APP_NAME%.config.h
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

arduino-cli.exe compile -b arduino:avr:mega:cpu=atmega2560 HeatingControl.ino -v -e --build-property "build.extra_flags=\"-DAPP_%APP_NAME%=1\""
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

rmdir build\%APP_NAME% /q /S
rename build\arduino.avr.mega %APP_NAME%
