/*
 * tHeatingCircleControl.h
 *
 *  Created on: 11 pa� 2022
 *      Author: szkud
 */

#pragma once

#if CONFIG_HEATING_CIRCLE_CONTROL

#include "../global.h"
#include "tOutputProcessheatingControl.h"
#include "Common_code/sensors/tSensorHub.h"


// note that the events are triggered by a sensor thold
// set sensor to proper time
// than set the object as a callback to the se
class tHeatingCircleControl : public tSensorHubEvent {
public:
	static const uint16_t MAX_VALVE_TIME = 100;
	static const uint16_t MAX_PUMP_TIME = 100;

	static const uint8_t PAUSE_PREVENTION_CYCLES = 5;  // TODO - provide! 25sec now

   tHeatingCircleControl(
            char* ValveTempSensorSerial,
            char* HeatSourceSensorSerial,
            char* HeatStorageSensorSerial,
            uint8_t ValveOpenOutId,
            uint8_t ValveCloseOutId,
            uint8_t PumpOutId,
            uint16_t SlowValveTime)
      : mTargetTemp(0),
        mValveTempSensorDevID(255),
        mHeatSourceSensorDevID(255),
        mHeatStorageSensorDevID(255),
        mValveTempSensorSerial(ValveTempSensorSerial),
        mHeatSourceSensorSerial(HeatSourceSensorSerial),
        mHeatStorageSensorSerial(HeatStorageSensorSerial),
		  mTolerance(0),
		  mSlowValveTime(SlowValveTime),
		  mFastValveMoveThold(0),
		  mValveOpenOutId(ValveOpenOutId),
		  mValveCloseOutId(ValveCloseOutId),
		  mHisteresis(0),
		  mPumpOutId(PumpOutId),
		  mPausePreventionCycles(0),
		  mState(STATE_DISABLED) {}

   void Disable() { mState = STATE_DISABLED; }
   void Stop()  { mState = STATE_OFF; }
   void Start()  { mState = STATE_IDLE; mPausePreventionCycles = PAUSE_PREVENTION_CYCLES; }

   virtual void onEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob);

   void setTargetTemp(float TargetTemp) { mTargetTemp = TargetTemp * 10; }
   void setTolerance(float Tolerance)   { mTolerance = Tolerance * 10; }
   void setFastThold(float FastValveMoveThold) { mFastValveMoveThold = FastValveMoveThold * 10; }
   void setHisteresis(float Histeresis)   { mHisteresis = Histeresis * 10; }
   static void setPumpStopTemp(float StopTemp) {mPumpStopTempThold = StopTemp * 10; }
   static void setPumpStartTemp(float StartTemp) {mPumpStartTempThold = StartTemp * 10; }

   float getTargetTemp() const { return ((float)(mTargetTemp) / 10); }
   float getTolerance()  const { return ((float)(mTolerance) / 10); }
   float getFastThold()  const { return ((float)(mFastValveMoveThold) / 10); }
   float getHisteresis() const { return ((float)(mHisteresis) / 10); }
   static float getPumpStopTemp() { return ((float)(mPumpStopTempThold) / 10); }
   static float getPumpStartTemp() { return ((float)(mPumpStartTempThold) / 10); }

   bool isWorking() const { return ((mState != STATE_OFF) && (mState != STATE_DISABLED)); }

private:
   typedef enum
   {
	   STATE_DISABLED,
	   STATE_OFF,
	   STATE_IDLE,
	   STATE_OPENING,
	   STATE_CLOSING,
	   STATE_PAUSED
   } tState;

   static int16_t mPumpStopTempThold;
   static int16_t mPumpStartTempThold;
   uint8_t mPausePreventionCycles;
   tState mState;

   char *mValveTempSensorSerial;
   char *mHeatSourceSensorSerial;
   char *mHeatStorageSensorSerial;
   uint8_t mValveTempSensorDevID;
   uint8_t mHeatSourceSensorDevID;
   uint8_t mHeatStorageSensorDevID;
   int16_t mTargetTemp;			   //< target temperature (*10)
   int16_t mFastValveMoveThold;	//< delta thold when the valve is triggered at full speed
   int16_t mTolerance;           //< delta thold when the temp correction stops
   int16_t mHisteresis;          //< delta thols when the correction starts
   uint8_t mValveOpenOutId;
   uint8_t mValveCloseOutId;
   uint8_t mPumpOutId;
   uint8_t mSlowValveTime;       //< num of seconds the valve is opened in a single step when moving at slow speed

   void StopValve()  { DEBUG_PRINTLN_3("==>STOP");
      tOutputProcess::get()->SetOutput(mValveOpenOutId, 0);  tOutputProcess::get()->SetOutput(mValveCloseOutId, 0); }
   void OpenValve()  { DEBUG_PRINTLN_3("==>OPEN");
      tOutputProcess::get()->SetOutput(mValveCloseOutId, 0); tOutputProcess::get()->SetOutput(mValveOpenOutId, 1, MAX_VALVE_TIME); }
   void CloseValve() { DEBUG_PRINTLN_3("==>CLOSE");
      tOutputProcess::get()->SetOutput(mValveOpenOutId, 0);  tOutputProcess::get()->SetOutput(mValveCloseOutId,1, MAX_VALVE_TIME); }
   void OpenValveSlow()  { DEBUG_PRINTLN_3("==>OPEN SLOW");
      tOutputProcess::get()->SetOutput(mValveCloseOutId, 0); tOutputProcess::get()->SetOutput(mValveOpenOutId, 1,mSlowValveTime); }
   void CloseValveSlow() { DEBUG_PRINTLN_3("==>CLOSE SLOW");
      tOutputProcess::get()->SetOutput(mValveOpenOutId, 0);  tOutputProcess::get()->SetOutput(mValveCloseOutId, 1,mSlowValveTime); }

   void PumpOn()  { DEBUG_PRINTLN_3("==>PUMP ON"); tOutputProcess::get()->SetOutput(mPumpOutId, 1, MAX_VALVE_TIME); }
   void PumpOff() { DEBUG_PRINTLN_3("==>PUMP OFF"); tOutputProcess::get()->SetOutput(mPumpOutId, 0); }
};
#endif //CONFIG_HEATING_CIRCLE_CONTROL
