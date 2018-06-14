#ifndef TYPES_H  
#define TYPES_H

#include "Time.h"

#define CHARS_MAX 10
#define NVM_INVALID_ADDRESS 1024
#define NVM_TIME_HOUR_ADDRESS 2000
#define NVM_TIME_MIN_ADDRESS 2001
#define NVM_TIME_YEAR_ADDRESS 2002
#define NVM_TIME_MONTH_ADDRESS 2003
#define NVM_TIME_DAY_ADDRESS 2004

enum OutputSetting_Enum {
  SETTING_OUT_AMOUNT,
  SETTING_OUT_CYCLE,
  SETTING_OUT_DAYTIME,
  SETTING_OUT_OFFSET,
  SETTING_OUT_TESTRUN,
  SETTING_OUT_MAX
};

enum WaterOutlet_Enum {
  WATER_OUTLET_1,
  WATER_OUTLET_2,
  WATER_OUTLET_3,
  WATER_OUTLET_4,
  WATER_OUTLET_MAX
};

typedef struct ValueType {
  int16_t value;
  int16_t step;;
  int8_t min;
  int16_t max;
  char unit[4];
  char name[CHARS_MAX];
  uint16_t nvmAddress;
} ValueType;


struct TimeValueType {
  ValueType hour;
  ValueType min;
  ValueType year;
  ValueType month;
  ValueType day;
};

struct DisplayType {
  /*ValueType onTime;*/
};

struct WateringBoy_DataType{
  ValueType out[WATER_OUTLET_MAX][SETTING_OUT_MAX];
  DisplayType display;
  TimeValueType time;
} WB;

void setSetting(ValueType* val, int32_t value, int32_t step, int32_t min, int32_t max, const char unit[], const char name[], uint16_t address) {
  val->value = value;
  val->step = step;
  val->min = min;
  val->max = max;
  val->nvmAddress = address;
  strcpy(val->unit, unit);
  strcpy(val->name, name);
}

void init_time(TimeType* time) {
  setSetting(&WB.time.hour,  time->hour,   1,  0,    23,   "h",    "Hour", NVM_TIME_HOUR_ADDRESS);
  setSetting(&WB.time.min,   time->minute, 1,  0,    59, "min", "Minutes", NVM_TIME_MIN_ADDRESS);
  
  setSetting(&WB.time.year,  time->year,   1,  0,    99,   "a",    "Year", NVM_TIME_YEAR_ADDRESS);
  setSetting(&WB.time.month, time->month,  1,  1,    12,   "m",   "Month", NVM_TIME_MONTH_ADDRESS);
  setSetting(&WB.time.day,   time->day,    1,  1,    31,   "d",     "Day", NVM_TIME_DAY_ADDRESS);
}
void init_setting() {
  /* Outlet 1 */
  setSetting(&WB.out[WATER_OUTLET_1][SETTING_OUT_AMOUNT], 200, 50, 10, 1000,  "ml",  "Amount", 0);
  setSetting(&WB.out[WATER_OUTLET_1][SETTING_OUT_CYCLE],   24,  3,  3, 5040,   "h",   "Cycle", 4);
  setSetting(&WB.out[WATER_OUTLET_1][SETTING_OUT_DAYTIME], 17,  1,  0,   23, "Uhr", "Daytime", 8);
  setSetting(&WB.out[WATER_OUTLET_1][SETTING_OUT_OFFSET],   0, 20,  0,  500,  "ml",  "Offset", 12);
  setSetting(&WB.out[WATER_OUTLET_1][SETTING_OUT_TESTRUN],  0,  1,  0,    1,    "", "Testrun", NVM_INVALID_ADDRESS);

  /* Outlet 2 */
  setSetting(&WB.out[WATER_OUTLET_2][SETTING_OUT_AMOUNT], 500, 50, 10, 1000,  "ml",  "Amount", 16);
  setSetting(&WB.out[WATER_OUTLET_2][SETTING_OUT_CYCLE],   24,  3,  3, 5040,   "h",   "Cycle", 20);
  setSetting(&WB.out[WATER_OUTLET_2][SETTING_OUT_DAYTIME], 17,  1,  0,   23, "Uhr", "Daytime", 24);
  setSetting(&WB.out[WATER_OUTLET_2][SETTING_OUT_OFFSET],   0, 20,  0,  500,  "ml",  "Offset", 28);
  setSetting(&WB.out[WATER_OUTLET_2][SETTING_OUT_TESTRUN],  0,  1,  0,    1,    "", "Testrun", NVM_INVALID_ADDRESS);

  /* Outlet 3 */
  setSetting(&WB.out[WATER_OUTLET_3][SETTING_OUT_AMOUNT], 300, 50, 10, 1000,  "ml",  "Amount", 32);
  setSetting(&WB.out[WATER_OUTLET_3][SETTING_OUT_CYCLE],   48,  3,  3, 5040,   "h",   "Cycle", 36);
  setSetting(&WB.out[WATER_OUTLET_3][SETTING_OUT_DAYTIME], 17,  1,  0,   23, "Uhr", "Daytime", 40);
  setSetting(&WB.out[WATER_OUTLET_3][SETTING_OUT_OFFSET],   0, 20,  0,  500,  "ml",  "Offset", 44);
  setSetting(&WB.out[WATER_OUTLET_3][SETTING_OUT_TESTRUN],  0,  1,  0,    1,    "", "Testrun", NVM_INVALID_ADDRESS);
  
  /* Outlet 4 */
  setSetting(&WB.out[WATER_OUTLET_4][SETTING_OUT_AMOUNT], 200, 50, 10, 1000,  "ml",  "Amount", 48);
  setSetting(&WB.out[WATER_OUTLET_4][SETTING_OUT_CYCLE],  168,  3,  3, 5040,   "h",   "Cycle", 52);
  setSetting(&WB.out[WATER_OUTLET_4][SETTING_OUT_DAYTIME], 17,  1,  0,   23, "Uhr", "Daytime", 56);
  setSetting(&WB.out[WATER_OUTLET_4][SETTING_OUT_OFFSET],   0, 20,  0,  500,  "ml",  "Offset", 60);
  setSetting(&WB.out[WATER_OUTLET_4][SETTING_OUT_TESTRUN],  0,  1,  0,    1,    "", "Testrun", NVM_INVALID_ADDRESS);
}

#endif /*TYPES_H*/
