#include <EEPROM.h>
#include "types.h"
#include "Time.h"

namespace NVM 
{
  static void EEPROMWritelong(int address, long value);
  static long EEPROMReadlong(long address);
  static void saveValue(ValueType* value);
  static void loadValue(ValueType* value);
  static void loadAllValues();
  
  static void saveValue(ValueType* value) {
    if (value->nvmAddress != NVM_INVALID_ADDRESS) {
      switch(value->nvmAddress ) {
        case NVM_TIME_HOUR_ADDRESS: Clock.setHour(value->value);    break;
        case NVM_TIME_MIN_ADDRESS:  Clock.setMinute(value->value);  break;
        case NVM_TIME_YEAR_ADDRESS: Clock.setYear(value->value);    break;
        case NVM_TIME_MONTH_ADDRESS:Clock.setMonth(value->value);   break;
        case NVM_TIME_DAY_ADDRESS:  Clock.setDate(value->value);    break;
        default: EEPROMWritelong(value->nvmAddress, value->value);  break;
      }
    }
  }
  
  static void loadValue(ValueType* value) {
    int16_t val = EEPROMReadlong(value->nvmAddress);
    val = constrain(val, value->min, value->max);
    value->value = val;
  }
  
  static void loadAllValues() {
    for(uint8_t idxOut=0; idxOut < WATER_OUTLET_MAX; idxOut++) {
      loadValue(&WB.out[idxOut][SETTING_OUT_AMOUNT]);
      loadValue(&WB.out[idxOut][SETTING_OUT_CYCLE]);
      loadValue(&WB.out[idxOut][SETTING_OUT_DAYTIME]);
      loadValue(&WB.out[idxOut][SETTING_OUT_OFFSET]);
    }
  }

  #if defined(FIRST_RUN)
  static void writeAllValues() {
    for(uint8_t idxOut=0; idxOut < WATER_OUTLET_MAX; idxOut++) {
      saveValue(&WB.out[idxOut][SETTING_OUT_AMOUNT]);
      saveValue(&WB.out[idxOut][SETTING_OUT_CYCLE]);
      saveValue(&WB.out[idxOut][SETTING_OUT_DAYTIME]);
      saveValue(&WB.out[idxOut][SETTING_OUT_OFFSET]);
    }
  }
  #endif

  static void EEPROMWritelong(int address, long value) {
    byte four = (value & 0xFF);
    byte three = ((value >> 8) & 0xFF);
    byte two = ((value >> 16) & 0xFF);
    byte one = ((value >> 24) & 0xFF);
    EEPROM.write(address, four);
    EEPROM.write(address + 1, three);
    EEPROM.write(address + 2, two);
    EEPROM.write(address + 3, one);
  }

  static long EEPROMReadlong(long address) {
    //Read the 4 bytes from the eeprom memory.
    long four = EEPROM.read(address);
    long three = EEPROM.read(address + 1);
    long two = EEPROM.read(address + 2);
    long one = EEPROM.read(address + 3);
  
    //Return the recomposed long by using bitshift.
    return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
  }
}

