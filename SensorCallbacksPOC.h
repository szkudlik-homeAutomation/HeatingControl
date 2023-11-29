/*
 * SensorCallbacksPOC.h
 *
 *  Created on: 29 lis 2023
 *      Author: szkud
 */

#pragma once


class tDS1820SensorCallback : public tMessageReciever
{
public:
   tDS1820SensorCallback() : tMessageReciever()
   	   { RegisterMessageType(MessageType_SensorEvent); }

   virtual void onMessage(uint8_t type, uint16_t data, void *pData)
   {
	   uint8_t SensorID = data;
	   if (type != MessageType_SensorEvent)
		   return;
	   tSensorEvent *pSensorEvent = (tSensorEvent *)pData;
	   if (pSensorEvent->SensorType != SENSOR_TYPE_DS1820)
		   return;
	   if (pSensorEvent->EventType == EV_TYPE_MEASUREMENT_ERROR)
	   {
	       DEBUG_PRINT_3("Measurement error. SensorID: ");
	       DEBUG_3(println(SensorID));
	       return;
	   }

	   if (pSensorEvent->EventType != EV_TYPE_MEASUREMENT_COMPLETED)
		   return;

	   tDS1820Sensor::tResult *pResult = (tDS1820Sensor::tResult *)pSensorEvent->pDataBlob;
       DEBUG_PRINT_3("Measurement completed. SensorID: ");
       DEBUG_3(print(SensorID));
       DEBUG_PRINT_3(" devs: ");
       DEBUG_3(print(pResult->NumOfDevices));
       DEBUG_PRINT_3(" Avg: ");
       DEBUG_3(print(pResult->Avg));
       uint8_t NumOfItems;
       if (pResult->Avg) {
          NumOfItems = 1;
       }
       else
       {
          NumOfItems = pResult->NumOfDevices;
       }
       for (int i = 0; i < NumOfItems; i++)
       {
          DEBUG_PRINT_3(" dev: ");
          DEBUG_3(print(i));
          DEBUG_PRINT_3(" temp: ");
          DEBUG_3(print(((float)(pResult)->Dev[i].Temperature) / 10));
       }
       DEBUG_PRINTLN_3("");
   }
};

class tImpulseSensorCallback : public tMessageReciever
{
public:
   tImpulseSensorCallback(uint8_t SensorID): mSensorID(SensorID), tMessageReciever()
   	   { RegisterMessageType(MessageType_SensorEvent); }
   virtual void onMessage(uint8_t type, uint16_t data, void *pData)
   {
	   uint8_t SensorID = data;
	   if (type != MessageType_SensorEvent)
		   return;
	   if (data != mSensorID)
		   return;
	   tSensorEvent *pSensorEvent = (tSensorEvent *)pData;
	   if (pSensorEvent->EventType != EV_TYPE_MEASUREMENT_COMPLETED)
		   return;

	   tImpulseSensor::tResult *pResult = (tImpulseSensor::tResult *)pSensorEvent->pDataBlob;

       DEBUG_PRINT_3("SensorID: ");
       DEBUG_3(print(SensorID));
       DEBUG_PRINT_3(" impulse count: ");
       DEBUG_3(print(pResult->Count));
       DEBUG_PRINT_3(" Sum: ");
       DEBUG_3(println(pResult->Sum));
   }
private:
   uint8_t mSensorID;
};

class tPt100SensorCallback : public tMessageReciever
{
public:
   tPt100SensorCallback(uint8_t SensorID) : mSensorID(SensorID), tMessageReciever()
   	   { RegisterMessageType(MessageType_SensorEvent); }
   virtual void onMessage(uint8_t type, uint16_t data, void *pData)
   {
	   uint8_t SensorID = data;
	   if (type != MessageType_SensorEvent)
		   return;
	   if (data != mSensorID)
		   return;
	   tSensorEvent *pSensorEvent = (tSensorEvent *)pData;
	   if (pSensorEvent->EventType != EV_TYPE_MEASUREMENT_COMPLETED)
		   return;

	   tPt100AnalogSensor::tResult *pResult = (tPt100AnalogSensor::tResult *)pSensorEvent->pDataBlob;

	   DEBUG_PRINT_3("PT100 Temp: ");
	   DEBUG_3(println(pResult->Temperature));
   }
private:
   uint8_t mSensorID;
};
