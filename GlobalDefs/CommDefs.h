#pragma once

// maximum number of nodes, should not be mode than 32 because of bitmaps
#define MAX_NUM_OF_NODES 32

/**
 * force the note to reset. May be sent as a broadcast
 */
#define MESSAGE_TYPE_FORCE_RESET 0x01

/**
 * dest node must response with MESSAGE_TYPE_FW_VERSION_RESPONSE  to the central node
 * no payload data
 */
#define MESSAGE_TYPE_FW_VERSION_REQUEST 0x02

/**
 * firmware version
 */
#define MESSAGE_TYPE_FW_VERSION_RESPONSE 0x03
typedef struct
{
  uint8_t Major;
  uint8_t Minor;
  uint8_t Patch;
} tMessageTypeFwVesionResponse;

/**
 * request the state of all outputs from a device
 * device should respoind with MESSAGE_TYPE_OVERVIEW_STATE_RESPONSE
 * 
 */
#define MESSAGE_TYPE_OVERVIEW_OUTPUT_STATE_REQUEST 0x04

/**
 * Sent on request - actual state of all outputs
 * sent to the device requesting it by MESSAGE_TYPE_OVERVIEW_STATE_REQUEST 
 */
#define MESSAGE_TYPE_OVERVIEW_OUTPUT_STATE_RESPONSE 0x05
typedef struct 
{
  uint8_t  PowerState;      // state of 8 outputs, 1 means it is on 
  uint8_t  TimerState;      // state of 8 timers, 1 means the output is on but there's timer pending, no timer value here
} tMessageTypeOverviewOutputStateResponse;


/**
 * request the state of an ouput from a device
 * device should respoind with MESSAGE_TYPE_OUTPUT_STATE_REQUEST
 */
#define MESSAGE_TYPE_OUTPUT_STATE_REQUEST 0x06
typedef struct 
{
  uint8_t  OutputID;      // id of an output 
} tMessageTypeOutputStateRequest;

/**
 * Sent on request - actual state of an output
 * sent to the device requesting it by MESSAGE_TYPE_OUTPUT_STATE_RESPONSE 
 */
#define MESSAGE_TYPE_OUTPUT_STATE_RESPONSE 0x07
typedef struct 
{
  uint8_t  OutputID;        // id of an output
  uint8_t  PowerState;      // state - 1 means on
  uint16_t TimerValue;      // state of a timer. 0 means there's no timer pending
  uint16_t DefaultTimer;    // a default timer for the output. 
} tMessageTypeOutputStateResponse;

/**
 * Sent by the central node - arbitrary set state of a single output
 */
#define MESSAGE_TYPE_SET_OUTPUT 0x08
typedef struct 
{
  uint8_t  OutId;        // the output id to be set
  uint8_t  State;        // state 0 or 1 to be set, where "1" means an active state
  uint16_t Timer;        // timer when the output should be turned off. In seconds, 0 means forever, 0xFFFF means default timer value
} tMessageTypeSetOutput;

/**
 * Look for a sensor with a given ID
 * A node that holds the sensor in question must response with MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE
 */
#define MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST 0x09
typedef struct
{
	uint8_t SensorID;
} tMessageGetSensorByIdReqest;

/**
 * Look for a sensor with a given ID
 * A node that holds the sensor in question must response with MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE
 */
#define MESSAGE_TYPE_GET_SENSOR_BY_ID 0x0A
typedef struct
{
	uint8_t Status;
} tMessageGetSensorByIdResponse;

