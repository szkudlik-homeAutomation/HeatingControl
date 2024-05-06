#pragma once
#include "../global.h"
#include <EEPROM.h>


#define EEPROM_CANNARY 0xA0     // change if update changes EEPROM structure

typedef struct
{
    uint8_t sensorType;
    uint8_t sensorID;
    uint8_t configBlobApiVersion;     // api version that has beed saved
    uint16_t measurementPeriod;
    uint8_t eventMask;
    uint8_t nameSize;
    uint8_t configBlobSize;
    uint8_t name[0];    // without NULL delimiter
    uint8_t configBlob[0];
} tEepromSensorEntry;

typedef struct __tEeprom
{
  uint8_t DeviceID;  // MUST be at the 1st position
  uint8_t CANNARY;   // a cannary, if is not set to EEPROM_CANNARY the eeprom should be set to default
#if CONFIG_NETWORK
  uint8_t IP[4];
  uint8_t IPMask[4];
  uint8_t Gateway[4];
  uint8_t DNS[4];
  uint8_t MAC[6];
#endif CONFIG_NETWORK
  uint8_t numOfSensors;
  tEepromSensorEntry Sensor[0];
};

#define EEPROM_DEVICE_ID_OFFSET offsetof(__tEeprom,DeviceID)

#define EEPROM_CANNARY_OFFSET offsetof(__tEeprom,CANNARY)

#if CONFIG_NETWORK
#define EEPROM_IP offsetof(__tEeprom,IP[0])
#define EEPROM_IPMASK offsetof(__tEeprom,IPMask[0])
#define EEPROM_GATEWAY offsetof(__tEeprom,Gateway[0])
#define EEPROM_DNS offsetof(__tEeprom,DNS[0])
#define EEPROM_MAC offsetof(__tEeprom,MAC[0])
#endif //CONFIG_NETWORK
#define EEPROM_NUM_OF_SENSORS offsetof(__tEeprom,numOfSensors)
#define EEPROM_FIRST_SENSOR offsetof(__tEeprom, Sensor[0])

void SetDefaultEEPromValues();
void eepromDeleteAllSensors();
