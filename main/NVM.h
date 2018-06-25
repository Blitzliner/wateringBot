#include <EEPROM.h>
#include "types.h"
#include "Time.h"

namespace NVM 
{
    /*************************************************/
    /*           CONSTANT DECLARATION                */
    /*************************************************/
    #define NVM_INVALID_ADDRESS 1024
    #define NVM_TIME_HOUR_ADDRESS 2000
    #define NVM_TIME_MIN_ADDRESS 2001
    #define NVM_TIME_YEAR_ADDRESS 2002
    #define NVM_TIME_MONTH_ADDRESS 2003
    #define NVM_TIME_DAY_ADDRESS 2004

    /*************************************************/
    /*         LOCAL FUNCTION DECLARATION            */
    /*************************************************/
    static void EepromWrite(uint16_t address, int16_t value);
    static int16_t EepromRead(uint16_t address);
    static void SetValue(ValueType* value);
    static void GetValue(ValueType* value);
    static void GetAllValues();
    static void SetSetting(ValueType* val, int32_t value, int32_t step, int32_t min, int32_t max, const char* unit_pc, const char* name_pc, uint16_t address);
    static void SettingTime_Init(TimeType* time);
    static void SettingDisplay_Init(void);
    static void SettingOutlets_Init(void);
    static void SettingOverallOutlets_Init(void);

    /*************************************************/
    /*         GLOBAL FUNCTION DECLARATION           */
    /*************************************************/
    void Setting_Init(TimeType* time);


    /*************************************************/
    /*         LOCAL FUNCTION DEFINITIONS            */
    /*************************************************/
    static void SetValue(ValueType* value) {
        if (value->NnvmAdress_u16 != NVM_INVALID_ADDRESS) {
            switch(value->NnvmAdress_u16 ) {
            case NVM_TIME_HOUR_ADDRESS: Clock.setHour(value->Value_s16);    break;
            case NVM_TIME_MIN_ADDRESS:  Clock.setMinute(value->Value_s16);  break;
            case NVM_TIME_YEAR_ADDRESS: Clock.setYear(value->Value_s16);    break;
            case NVM_TIME_MONTH_ADDRESS:Clock.setMonth(value->Value_s16);   break;
            case NVM_TIME_DAY_ADDRESS:  Clock.setDate(value->Value_s16);    break;
            default: EepromWrite(value->NnvmAdress_u16, value->Value_s16);  break;
            }
        }
    }
  
    static void GetValue(ValueType* value) {
        int16_t val = EepromRead(value->NnvmAdress_u16);
        val = constrain(val, value->Min_s16, value->Max_s16);
        value->Value_s16 = val;
    }
  
    static void GetAllValues() {
        for(uint8_t idxOut_u8=0; idxOut_u8 < WATER_OUTLET_MAX; idxOut_u8++) {
            GetValue(&WB.Out_as[idxOut_u8][SETTING_OUT_AMOUNT]);
            GetValue(&WB.Out_as[idxOut_u8][SETTING_OUT_CYCLE]);
            GetValue(&WB.Out_as[idxOut_u8][SETTING_OUT_DAYTIME]);
            GetValue(&WB.Out_as[idxOut_u8][SETTING_OUT_OFFSET]);
        }
        GetValue(&WB.Display_s.ScreenSaver_s);
        GetValue(&WB.Display_s.Sleep_s);
    }

    #if defined(FIRST_RUN)
    static void writeAllValues() {
        for(uint8_t idxOut=0; idxOut < WATER_OUTLET_MAX; idxOut++) {
            saveValue(&WB.out[idxOut][SETTING_OUT_AMOUNT]);
            saveValue(&WB.out[idxOut][SETTING_OUT_CYCLE]);
            saveValue(&WB.out[idxOut][SETTING_OUT_DAYTIME]);
            saveValue(&WB.out[idxOut][SETTING_OUT_OFFSET]);
        }
        saveValue(&WB.Display.ScreenSaver);
        saveValue(&WB.Display.Sleep);
    }
    #endif

    static void EepromWrite(uint16_t address, int16_t value) {
        EEPROM.write(address, (value & 0xFF));
        EEPROM.write(address + 1, ((value >> 8) & 0xFF));
    }

    static int16_t EepromRead(uint16_t address) {
        int16_t four = EEPROM.read(address);
        int16_t three = EEPROM.read(address + 1);  
        return ((four & 0xFF) + ((three << 8) & 0xFFFF));
    }


    static void SetSetting(ValueType* val, int32_t value, int32_t step, int32_t min, int32_t max, const char* unit_pc, const char* name_pc, uint16_t address) {
        val->Value_s16 = value;
        val->Step_s16 = step;
        val->Min_s16 = min;
        val->Max_s16 = max;
        val->NnvmAdress_u16 = address;
        val->Unit_pc = unit_pc;
        val->Name_pc = name_pc;
    }

    static void SettingTime_Init(TimeType* time) {
        SetSetting(&WB.Time_s.Hour_s, time->hour, 1, 0, 23, Names::Unit::Hour, Names::Setting::Hour, NVM_TIME_HOUR_ADDRESS);
        SetSetting(&WB.Time_s.Min_s, time->minute, 1, 0, 59, Names::Unit::Minute, Names::Setting::Minutes, NVM_TIME_MIN_ADDRESS);

        SetSetting(&WB.Time_s.Year_s, time->year, 1, 0, 99, Names::Unit::Year, Names::Setting::Year, NVM_TIME_YEAR_ADDRESS);
        SetSetting(&WB.Time_s.Month_s, time->month, 1, 1, 12, Names::Unit::Month, Names::Setting::Month, NVM_TIME_MONTH_ADDRESS);
        SetSetting(&WB.Time_s.Day_s, time->day, 1, 1, 31, Names::Unit::Day, Names::Setting::Day, NVM_TIME_DAY_ADDRESS);
    }

    static void SettingDisplay_Init(void) {
        SetSetting(&WB.Display_s.ScreenSaver_s, 10, 1, 5, 60, Names::Unit::Second, Names::Setting::ScreenSaver, 64);
        SetSetting(&WB.Display_s.Sleep_s, 20, 5, 10, 3600, Names::Unit::Second, Names::Setting::Standby, 68);
    }

    static void SettingOverallOutlets_Init(void) {
        SetSetting(&WB.Outlets_s.FlowAmount_s, 50, 1, 5, 200, Names::Unit::Flow, Names::Setting::FlowAmount, 72);
        SetSetting(&WB.Outlets_s.Enable_s, 15, 1, 1, 15, Names::Unit::Second, Names::Setting::Enable, 74);
    }

    static void SettingOutlets_Init(void) {
        /* Outlet 1 */
        SetSetting(&WB.Out_as[WATER_OUTLET_1][SETTING_OUT_AMOUNT], 200, 50, 10, 1000, Names::Unit::Milliliter, Names::Setting::Amount, 0);
        SetSetting(&WB.Out_as[WATER_OUTLET_1][SETTING_OUT_CYCLE], 24, 3, 3, 5040, Names::Unit::Hour, Names::Setting::Cycle, 4);
        SetSetting(&WB.Out_as[WATER_OUTLET_1][SETTING_OUT_DAYTIME], 17, 1, 0, 23, Names::Unit::Clock, Names::Setting::Daytime, 8);
        SetSetting(&WB.Out_as[WATER_OUTLET_1][SETTING_OUT_OFFSET], 0, 20, 0, 500, Names::Unit::Milliliter, Names::Setting::Offset, 12);
        SetSetting(&WB.Out_as[WATER_OUTLET_1][SETTING_OUT_TESTRUN], 0, 1, 0, 1, Names::Unit::NoUnit, Names::Setting::Testrun, NVM_INVALID_ADDRESS);

        /* Outlet 2 */
        SetSetting(&WB.Out_as[WATER_OUTLET_2][SETTING_OUT_AMOUNT], 500, 50, 10, 1000, Names::Unit::Milliliter, Names::Setting::Amount, 16);
        SetSetting(&WB.Out_as[WATER_OUTLET_2][SETTING_OUT_CYCLE], 24, 3, 3, 5040, Names::Unit::Hour, Names::Setting::Cycle, 20);
        SetSetting(&WB.Out_as[WATER_OUTLET_2][SETTING_OUT_DAYTIME], 17, 1, 0, 23, Names::Unit::Clock, Names::Setting::Daytime, 24);
        SetSetting(&WB.Out_as[WATER_OUTLET_2][SETTING_OUT_OFFSET], 0, 20, 0, 500, Names::Unit::Milliliter, Names::Setting::Offset, 28);
        SetSetting(&WB.Out_as[WATER_OUTLET_2][SETTING_OUT_TESTRUN], 0, 1, 0, 1, Names::Unit::NoUnit, Names::Setting::Testrun, NVM_INVALID_ADDRESS);

        /* Outlet 3 */
        SetSetting(&WB.Out_as[WATER_OUTLET_3][SETTING_OUT_AMOUNT], 300, 50, 10, 1000, Names::Unit::Milliliter, Names::Setting::Amount, 32);
        SetSetting(&WB.Out_as[WATER_OUTLET_3][SETTING_OUT_CYCLE], 48, 3, 3, 5040, Names::Unit::Hour, Names::Setting::Cycle, 36);
        SetSetting(&WB.Out_as[WATER_OUTLET_3][SETTING_OUT_DAYTIME], 17, 1, 0, 23, Names::Unit::Clock, Names::Setting::Daytime, 40);
        SetSetting(&WB.Out_as[WATER_OUTLET_3][SETTING_OUT_OFFSET], 0, 20, 0, 500, Names::Unit::Milliliter, Names::Setting::Offset, 44);
        SetSetting(&WB.Out_as[WATER_OUTLET_3][SETTING_OUT_TESTRUN], 0, 1, 0, 1, Names::Unit::NoUnit, Names::Setting::Testrun, NVM_INVALID_ADDRESS);

        /* Outlet 4 */
        SetSetting(&WB.Out_as[WATER_OUTLET_4][SETTING_OUT_AMOUNT], 200, 50, 10, 1000, Names::Unit::Milliliter, Names::Setting::Amount, 48);
        SetSetting(&WB.Out_as[WATER_OUTLET_4][SETTING_OUT_CYCLE], 168, 3, 3, 5040, Names::Unit::Hour, Names::Setting::Cycle, 52);
        SetSetting(&WB.Out_as[WATER_OUTLET_4][SETTING_OUT_DAYTIME], 17, 1, 0, 23, Names::Unit::Clock, Names::Setting::Daytime, 56);
        SetSetting(&WB.Out_as[WATER_OUTLET_4][SETTING_OUT_OFFSET], 0, 20, 0, 500, Names::Unit::Milliliter, Names::Setting::Offset, 60);
        SetSetting(&WB.Out_as[WATER_OUTLET_4][SETTING_OUT_TESTRUN], 0, 1, 0, 1, Names::Unit::NoUnit, Names::Setting::Testrun, NVM_INVALID_ADDRESS);
    }


    /*************************************************/
    /*         GLOBAL FUNCTION DEFINITIONS           */
    /*************************************************/
    void Setting_Init(TimeType* time) {
        SettingTime_Init(time);/* set default values, names, .. */
        SettingDisplay_Init();
        SettingOutlets_Init();
        SettingOverallOutlets_Init();
    }

}

