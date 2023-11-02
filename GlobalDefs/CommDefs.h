#pragma once

// maximum number of nodes, should not be mode than 32 because of bitmaps
#define MAX_NUM_OF_NODES 32

#define NUM_OF_RETRANSMISSIONS 1

#define MAX_NUM_OF_RETRANSMISSIONS 5

#define FRAME_TRANSMISSION_TIME 20

#define MAX_TRANSMIT_DELAY 100
#define OUTPUT_QUEUE_SIZE 3

#define COMMUNICATION_PAYLOAD_DATA_SIZE 8


/**
 * Look for a sensor with a given ID
 * A node that holds the sensor in question must response with MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE
 */
#define MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST 0x20
typedef struct
{
	uint8_t SensorID;
} tMessageGetSensorByIdReqest;
C_ASSERT(sizeof(tMessageGetSensorByIdReqest) <= COMMUNICATION_PAYLOAD_DATA_SIZE);

/**
 * Look for a sensor with a given ID
 * A node that holds the sensor in question must response with MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE
 */
#define MESSAGE_TYPE_GET_SENSOR_BY_ID 0x21
typedef struct
{
	uint8_t Status;
} tMessageGetSensorByIdResponse;
C_ASSERT(sizeof(tMessageGetSensorByIdResponse) <= COMMUNICATION_PAYLOAD_DATA_SIZE);
