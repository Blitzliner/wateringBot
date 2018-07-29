#ifndef TYPES_H  
#define TYPES_H

#include "Time.h"

/*************************************************/
/*           CONSTANT DECLARATION                */
/*************************************************/
//#define CHARS_MAX 10
#define INVALID_VALVE_IDX -1
/*************************************************/
/*              TYPE DECLARATION                 */
/*************************************************/
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
  int16_t Value_s16;
  int16_t Step_s16;
  int16_t Min_s16;
  int16_t Max_s16;
  const char* Unit_pc;/* ac[4];*/
  const char* Name_pc;/* char Name_ac[CHARS_MAX];*/
  uint16_t NnvmAdress_u16;
} ValueType;


struct TimeValueType {
  ValueType Hour_s;
  ValueType Min_s;
  ValueType Year_s;
  ValueType Month_s;
  ValueType Day_s;
};

struct DisplayType {
    ValueType Sleep_s;
    ValueType ScreenSaver_s;
};

struct OutletsType {
    ValueType FlowAmount_s;
    ValueType Enable_s;
};

typedef struct WateringBoy_DataType {
  ValueType Out_as[WATER_OUTLET_MAX][SETTING_OUT_MAX];
  DisplayType Display_s;
  TimeValueType Time_s;
  OutletsType Outlets_s;
} WateringBoy_DataType;


#endif /*TYPES_H*/
