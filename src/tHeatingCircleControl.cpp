/*
 * tHeatingCircleControl.cpp
 *
 *  Created on: 11 pa� 2022
 *      Author: szkud
 */

#include "../global.h"
#include "tHeatingCircleControl.h"
#include "Common_code/sensors/tDS1820Sensor.h"

#define LOCAL_DEBUG


void tHeatingCircleControl::onEvent(tSensor *pSensor, tEventType EventType)
{
	if (EventType == EV_TYPE_MEASUREMENT_ERROR)
	{
		StopValve();
		return;
	}
	if (EventType != EV_TYPE_MEASUREMENT_COMPLETED)
	{
		return;
	}

	tDS1820Sensor::tResult *pDS1820Result =(tDS1820Sensor::tResult *) pSensor->getMeasurementBlob();
	int16_t CurrentTemperature = (pDS1820Result+mSensorDevID)->Temp;
	int16_t Delta = abs(CurrentTemperature - mTargetTemp);
	bool doOpen = (mTargetTemp > CurrentTemperature);

	/*
	 * steps
	 *  - if disabled - do nothing
	 *  - if state is STATE_OFF - set valve to closing. STOP
	 *  - if Delta is bigger than mFastValveMoveThold - set the valve to opening/closing and set proper state (OPENING/CLOSING). STOP.
	 *  - if delta is smaller than mSlowValveTime:
	 *  	- if state is CLOSING and Temp is bigger than target ==> Set valve to close slow. STOP
	 *  	- if state is OPENING and Temp is smaller than target ==> Set valve to open slow. STOP
	 *  	- if state is IDLE and Delta is bigger than mTolerance - set the valve to opening/closing and set proper state (OPENING/CLOSING). STOP.
	 *
	 *  being here means - the temperature is OK,
	 *  	- stop the valve
	 *  	- set state to IDLE
	 */

#ifdef LOCAL_DEBUG
	   DEBUG_SERIAL.print("-->Temp: ");
	   DEBUG_SERIAL.print((float)(CurrentTemperature) / 10);
	   DEBUG_SERIAL.print(" Target: ");
	   DEBUG_SERIAL.print(getTargetTemp());
	   DEBUG_SERIAL.print(" Fast: ");
	   DEBUG_SERIAL.print(getFastThold());
	   DEBUG_SERIAL.print(" Tolerance: ");
	   DEBUG_SERIAL.print(getTolerance());
      DEBUG_SERIAL.print(" Histeresis: ");
      DEBUG_SERIAL.println(getHisteresis());
#endif

	if (mState == STATE_OFF)
	{
		// pump is off, valve is closed
		CloseValve();
		PumpOff();
		return;
	}

	if (mState == STATE_DISABLED)
	{
		// do nothing
		StopValve();
		PumpOff();
		return;
	}


	PumpOn();

	if ( (Delta > mFastValveMoveThold) ||
		((mState == STATE_IDLE) && (Delta > mHisteresis)) )
	{
		// big diffrence in temp or was in idle and run out of tolerance
		// force valve to open/close
		if (doOpen)
		{
			OpenValve();
			mState = STATE_OPENING;
		}
		else
		{
			CloseValve();
			mState = STATE_CLOSING;
		}

		return;
	}

	// delta must be smaller than mFastValveMoveThold

	if (Delta > mTolerance)
	{
      if ((mState == STATE_OPENING) && (doOpen))
      {
         OpenValveSlow();
         return;
      }

      if ((mState == STATE_CLOSING) && (! doOpen))
      {
         CloseValveSlow();
         return;
      }
	}
	// all other cases - switch the valve off
	StopValve();
	mState = STATE_IDLE;
}
