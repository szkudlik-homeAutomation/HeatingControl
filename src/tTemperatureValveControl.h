/*
 * tTemperatureValveControl.h
 *
 *  Created on: 11 paŸ 2022
 *      Author: szkud
 */

#ifndef SRC_TTEMPERATUREVALVECONTROL_H_
#define SRC_TTEMPERATUREVALVECONTROL_H_

#include "../global.h"
#include "Common_code/sensors/tDS1820Sensor.h"

class tTemperatureValveControl : public tSensorEvent {
public:
   tTemperatureValveControl(tDS1820Sensor *pSensor, uint8_t SensorDevID)
      : mpSensor(pSensor),
        mSensorDevID(SensorDevID)
        {
            pSensor->SetEventCalback(this);
        }

   virtual void onEvent(tSensor *pSensor, tEventType EventType);

private:
   tDS1820Sensor *mpSensor;
   uint8_t mSensorDevID;
};

#endif /* SRC_TTEMPERATUREVALVECONTROL_H_ */
