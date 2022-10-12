/*
 * tTemperatureValveControl.h
 *
 *  Created on: 11 pa� 2022
 *      Author: szkud
 */

#ifndef SRC_TTEMPERATUREVALVECONTROL_H_
#define SRC_TTEMPERATUREVALVECONTROL_H_

#include "../global.h"
#include "tOutputProcessheatingControl.h"
#include "Common_code/sensors/tSensor.h"

// note that the events are triggered by a sensor thold
// set sensor to proper time
// than set the object as a callback to the se
class tTemperatureValveControl : public tSensorEvent {
public:
	static const uint16_t MAX_VALVE_TIME = 100;

   tTemperatureValveControl(uint8_t SensorDevID, uint8_t ValveOpenOutId, uint8_t ValveCloseOutId, uint16_t SlowValveTime)
      : mTargetTemp(0),
        mSensorDevID(SensorDevID),
		mTolerance(0),
		mSlowValveTime(SlowValveTime),
		mFastValveMoveThold(0),
		mValveOpenOutId(ValveOpenOutId),
		mValveCloseOutId(ValveCloseOutId),
		mHisteresis(0),
		mState(STATE_DISABLED) {}

   void Disable() { mState = STATE_DISABLED; }
   void Enable()  { mState = STATE_IDLE; }

   virtual void onEvent(tSensor *pSensor, tEventType EventType);

   void setTargetTemp(float TargetTemp) { mTargetTemp = TargetTemp * 10; }
   void setTolerance(float Tolerance)   { mTolerance = Tolerance * 10; }
   void setFastThold(float FastValveMoveThold) { mFastValveMoveThold = FastValveMoveThold * 10; }
   void setHisteresis(float Histeresis)   { mHisteresis = Histeresis * 10; }

   float getTargetTemp() const { return ((float)(mTargetTemp) / 10); }
   float getTolerance()  const { return ((float)(mTolerance) / 10); }
   float getFastThold()  const { return ((float)(mFastValveMoveThold) / 10); }
   float getHisteresis() const { return ((float)(mHisteresis) / 10); }
private:
   typedef enum
   {
	   STATE_DISABLED,
	   STATE_IDLE,
	   STATE_OPENING,
	   STATE_CLOSING,
	   STATE_FORCE_CLOSE,
   } tState;

   tState mState;

   uint8_t mSensorDevID;
   int16_t mTargetTemp;			   //< target temperature (*10)
   int16_t mFastValveMoveThold;	//< delta thold when the valve is triggered at full speed
   int16_t mTolerance;           //< delta thold when the temp correction stops
   int16_t mHisteresis;          //< delta thols when the correction starts
   uint8_t mValveOpenOutId;
   uint8_t mValveCloseOutId;
   uint8_t mSlowValveTime;       //< num of seconds the valve is opened in a single step when moving at slow speed

   void StopValve()  { DEBUG_SERIAL.println("==>STOP");
      OutputProcess.SetOutput(mValveOpenOutId, 0);  OutputProcess.SetOutput(mValveCloseOutId, 0); }
   void OpenValve()  { DEBUG_SERIAL.println("==>OPEN");
      OutputProcess.SetOutput(mValveCloseOutId, 0); OutputProcess.SetOutput(mValveOpenOutId, 1, MAX_VALVE_TIME); }
   void CloseValve() { DEBUG_SERIAL.println("==>CLOSE");
      OutputProcess.SetOutput(mValveOpenOutId, 0);  OutputProcess.SetOutput(mValveCloseOutId,1, MAX_VALVE_TIME); }
   void OpenValveSlow()  { DEBUG_SERIAL.println("==>OPEN SLOW");
      OutputProcess.SetOutput(mValveCloseOutId, 0); OutputProcess.SetOutput(mValveOpenOutId, 1,mSlowValveTime); }
   void CloseValveSlow() { DEBUG_SERIAL.println("==>CLOSE SLOW");
      OutputProcess.SetOutput(mValveOpenOutId, 0);  OutputProcess.SetOutput(mValveCloseOutId, 1,mSlowValveTime); }
};

#endif /* SRC_TTEMPERATUREVALVECONTROL_H_ */
