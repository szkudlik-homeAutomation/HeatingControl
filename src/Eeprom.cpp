#include "../global.h"

void SetDefaultEEPromValues(bool force)
{

  if ((EEPROM.read(EEPROM_CANNARY_OFFSET) == EEPROM_CANNARY) && !force)
	  return;

  DEBUG_PRINTLN_3("!!!!!!!!!!!!!!!!!!!!!!!!! CLEAN EEPROM MEMORY - SETTING DEFAULTS !!!!!!!!!!!!!!!!!");
  EEPROM.write(EEPROM_CANNARY_OFFSET,EEPROM_CANNARY);

  // device ID is supposed to be pre-set for a node

#if CONFIG_NETWORK
  EEPROM.write(EEPROM_IP+0,192);
  EEPROM.write(EEPROM_IP+1,168);
  EEPROM.write(EEPROM_IP+2,101);
  EEPROM.write(EEPROM_IP+3,24);
  EEPROM.write(EEPROM_IPMASK+0,255);
  EEPROM.write(EEPROM_IPMASK+1,255);
  EEPROM.write(EEPROM_IPMASK+2,255);
  EEPROM.write(EEPROM_IPMASK+3,0);
  EEPROM.write(EEPROM_GATEWAY+0,192);
  EEPROM.write(EEPROM_GATEWAY+1,168);
  EEPROM.write(EEPROM_GATEWAY+2,101);
  EEPROM.write(EEPROM_GATEWAY+3,1);
  EEPROM.write(EEPROM_DNS+0,192);
  EEPROM.write(EEPROM_DNS+1,168);
  EEPROM.write(EEPROM_DNS+2,101);
  EEPROM.write(EEPROM_DNS+3,1);
  EEPROM.write(EEPROM_MAC+0,0x18);
  EEPROM.write(EEPROM_MAC+1,0xC0);
  EEPROM.write(EEPROM_MAC+2,0x4D);
  EEPROM.write(EEPROM_MAC+3,0x5A);
  EEPROM.write(EEPROM_MAC+4,0x5B);
  EEPROM.write(EEPROM_MAC+5,0x15);
#endif //CONFIG_NETWORK
}
