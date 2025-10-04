

#include "global.h"



#include "src/Common_code/sensors/tDS1820Sensor.h"
#include "src/Common_code/sensors/tImpulseSensor.h"
#include "src/Common_code/sensors/tPt100AnalogSensor.h"
#include "src/Common_code/sensors/tSimpleDigitalInputSensor.h"
#include "src/Common_code/sensors/tSystemStatusSensor.h"



#if CONFIG_LOGLEVEL_2
	tPt100SensorLogger Pt100SensorLogger(SENSOR_ID_PT100_HOTAIR);
	tDS1820SensorLogger DS1820SensorLogger(0);  // all DS1820s
	tImpulseSensorLogger ImpulseSensorLogger(SENSOR_ID_IMPULSE_HEATPUMP);
#endif // CONFIG_LOGLEVEL_2




