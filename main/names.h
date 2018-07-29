#include "types.h"

namespace Menu {
    typedef enum Menu_Enum {
        MAIN_MENU,
        OUTLET_MENU,
        OUTLET_DETAIL_1_MENU,
        OUTLET_DETAIL_2_MENU,
        OUTLET_DETAIL_3_MENU,
        OUTLET_DETAIL_4_MENU,
        OVERVIEW_MENU,
        SETTINGS_MENU, 
        SETTINGS_TIME_MENU,
        SETTINGS_DISPLAY_MENU,
        SETTINGS_OVERALL_OUTLET_MENU,
    } Menu_Enum;
  
    typedef enum MainMenu_Enum {
        MENU_MAIN_OUTPUT,
        MENU_MAIN_OVERVIEW,
        MENU_MAIN_SETTINGS,
        MENU_MAIN_MAX
    } MainMenu_Enum;

    typedef enum MenuOutlet_Enum {
        MENU_OUTLET_1,
        MENU_OUTLET_2,
        MENU_OUTLET_3,
        MENU_OUTLET_4,
        MENU_OUTLET_MAX
    } MenuOutlet_Enum;
  
    typedef enum MenuTime_Enum {
        MENU_TIME_HOUR,
        MENU_TIME_MINUTE,
        MENU_TIME_YEAR,
        MENU_TIME_MONTH,
        MENU_TIME_DAY,
        MENU_TIME_MAX
    } MenuTime_Enum;

    typedef enum MenuDisplay_Enum {
        MENU_DISPLAY_SCREENSAVER,
        MENU_DISPLAY_STANDBY,
        MENU_DISPLAY_MAX,
    } MenuDisplay_Enum;

    typedef enum MenuOverallOutlet_Enum {
        MENU_OVERALL_OUTLET_FLOW_AMOUNT,
        MENU_OVERALL_OUTLET_ENABLE,
        MENU_OVERALL_OUTLET_MAX,
    } MenuOverallOutlet_Enum;

    typedef enum MenuSettings_Enum {
        MENU_SET_TIME_DATE,
        MENU_SET_DISPLAY,
        MENU_SET_OVERALL_OUTLETS,
        MENU_SET_MAX
    } MenuSettings_Enum;

    typedef enum MainOutput_Enum {
        MENU_OUT_AMOUNT   = SETTING_OUT_AMOUNT,
        MENU_OUT_CYCLE    = SETTING_OUT_CYCLE,
        MENU_OUT_DAYTIME  = SETTING_OUT_DAYTIME,
        MENU_OUT_OFFSET   = SETTING_OUT_OFFSET,
        MENU_OUT_TESTRUN  = SETTING_OUT_TESTRUN,
        MENU_OUT_MAX      = SETTING_OUT_MAX
    } MainOutput_Enum;
}

namespace Names {
    const char Headline[] = "WateringBot";

    namespace Menu {
        const char Overview[] = "Overview";
        const char Settings[] = "Settings";
        const char Time[] = "Time";
        const char Display[] = "Display";
        const char Outlets[] = "Valves";
        const char Outlet[] = "Valve";
        const char Back[] = "back";
        const char Arrow[] = ">";
        const char Divide[] = "/";
        const char Space[] = " ";
    }

    namespace Setting {
        const char Hour[] = "Hour"; 
        const char Minutes[] = "Minutes";
        const char Year[] = "Year";
        const char Month[] = "Month";
        const char Day[] = "Day";
        const char ScreenSaver[] = "ScreenSaver";
        const char Standby[] = "Standby";
        const char FlowAmount[] = "FlowAmount";
        const char Enable[] = "Enable";
        const char Amount[] = "Amount";
        const char Cycle[] = "Cycle";
        const char Daytime[] = "Daytime";
        const char Offset[] = "Offset";
        const char Testrun[] = "Testrun";
    }

    namespace Unit {
        const char NoUnit[] = "";
        const char Hour[] = "h";
        const char Second[] = "s";
        const char Minute[] = "m";
        const char Milliliter[] = "ml";
        const char Day[] = "dd";
        const char Month[] = "mm";
        const char Year[] = "a";
        const char Flow[] = "ml/s";
        const char Clock[] = "Uhr";
    }
}


