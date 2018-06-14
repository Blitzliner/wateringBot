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
  } Menu_Enum;
  
  typedef enum MainMenu_Enum {
    MENU_MAIN_OUTPUT,
    MENU_MAIN_OVERVIEW,
    MENU_MAIN_SETTINGS,
    MENU_MAIN_MAX
  } MainMenu_Enum;

  typedef enum MainOutlet_Enum {
    MENU_OUTLET_1,
    MENU_OUTLET_2,
    MENU_OUTLET_3,
    MENU_OUTLET_4,
    MENU_OUTLET_MAX
  } MainOutlet_Enum;
  
  typedef enum MainTime_Enum {
    MENU_TIME_HOUR,
    MENU_TIME_MINUTE,
    MENU_TIME_YEAR,
    MENU_TIME_MONTH,
    MENU_TIME_DAY,
    MENU_TIME_MAX
  } MainTime_Enum;
  
  typedef enum MainSettings_Enum {
    MENU_SET_TIME_DATE,
    MENU_SET_DISPLAY,
    MENU_SET_MAX
  } MainSettings_Enum;

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
    char Headline[] = "WateringBot";
    char MainMenu[][CHARS_MAX] {
    "Outputs", "Overview", "Settings"
    };
    char OutletMenu[][CHARS_MAX] {
    "Outlet 1", "Outlet 2", "Outlet 3", "Outlet 4"
    };
    char SettingsMenu[][CHARS_MAX] {
    "Time", "Display"
    };
    char Back[] = "back";
    char Arrow[] = ">";
    char Divide[] = "/";
    char Space[] = " ";
}

