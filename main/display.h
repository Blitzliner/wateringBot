#include "types.h"
#include "displayWrapper.h"
#include "names.h"
#include "utils.h"
#include "NVM.h"

/*************************************************/
/*           CONSTANT DECLARATION                */
/*************************************************/
#define ENCODER_PIN_A 3
#define ENCODER_PIN_B 4
#define ENCODER_PIN_PRESS 2
#define OVERVIEW_MIN_DISPLAYED_DAYS 2
#define OVERVIEW_MAX_DISPLAYED_DAYS 10
#define EDIT_MODE_BLINK_DURATION 400
#define DISPLAY_VALUE_POSITION 60
#define DISPLAY_UNIT_POSITION 88

/*************************************************/
/*              TYPE DECLARATION                 */
/*************************************************/
typedef enum Key_EnumTag { KEY_CENTER, KEY_DOWN, KEY_UP, KEY_NONE } Key_EnumType;

typedef struct KeyPad_Tag {
    volatile uint8_t IsrValue_u8;
    volatile bool IsrPress_bo;
    Key_EnumType Key_e;
    boolean Clear_bo;
} KeyPad_Type;

typedef struct Menu_Tag {
    const uint8_t X_u8 = 8;
    const uint8_t Y_u8 = 8;
    uint8_t SelectedIdx_u8;
    Menu::Menu_Enum Selected_e;    
} Menu_Type;

typedef enum DISPLAY_MODE { 
    DISPLAY_NORMAL, 
    DISPLAY_SCREENSAVER, 
    DISPLAY_STANDBY, 
    DISPLAY_WATERING, 
    DISPLAY_INIT 
} DISPLAY_MODE;

typedef struct HMI_Tag {
    Menu_Type Menu_s;
    boolean EditMode_bo;
    ValueType* CurrentValue_p;
    uint8_t overviewNrDays_u8 = 3;
    KeyPad_Type KeyPad_s;
    int8_t CurrentOutlet_s8;
    uint16_t TotalMilliliter_u16;
    uint16_t CurrentAmount_u16;
    DISPLAY_MODE DisplayMode_e;
} HMI_Type;

typedef struct InputData_Tag {
    TimeType *Time_p; 
    WateringBoy_DataType *Wb_p;
} InputData_Type;


/*************************************************/
/*         LOCAL VARIABLE DECLARATION            */
/*************************************************/
static HMI_Type HmiData_s;
static InputData_Type InputData_p;

/*************************************************/
/*         LOCAL FUNCTION DECLARATION            */
/*************************************************/
static void setCursor(void);
static void showOutletMenu(void);
static void showSettingMenu(void);
static void showMainMenu(void);
static void ShowCursor_Main(void);
static void showOverview(void);
static void showValue(uint8_t posX, uint8_t posY, ValueType* value);
static void showDisplayDetail(void);
static void showCurrentWateringStatus(void);
static void showOverallOutletDetail(void);
static void showTimeDetail(void);
static void showOutletDetail(uint8_t out);
static void ShowMenu_Main(void);
static void enableEditMode(ValueType* val);
static void editValue(void);
static void Pins_Init(void);
static void KeyPad_Main();
void ISR_A(void);
void ISR_PRESS(void);

/*************************************************/
/*         GLOBAL FUNCTION DECLARATION           */
/*************************************************/
void Display_Init(TimeType* time_p);
void Display_Main(DISPLAY_MODE displayMode_e);
void ScreenSaver_Main(TimeType* time_p);
void DisplayUpdateWatering(int8_t currentOutlet, uint16_t totalMilliliter_u16, uint16_t currentAmount_u16);

/*************************************************/
/*         LOCAL FUNCTION DEFINITIONS            */
/*************************************************/
static void showMainMenu(void) {
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_1, Names::Headline);
    Display::HLine(Display::LINE_HORIZONTAL);
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_2, Names::Menu::Outlets);
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_3, Names::Menu::Overview);
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_4, Names::Menu::Settings);
}

static void showSettingMenu(void) {
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_1, Names::Menu::Settings);
    Display::HLine(Display::LINE_HORIZONTAL);
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_2, Names::Menu::Back);
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_3, Names::Menu::Time);
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_4, Names::Menu::Display);
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_5, Names::Menu::Outlets);
}

static void showOutletMenu(void) {
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_1, Names::Menu::Outlets);
    Display::HLine(Display::LINE_HORIZONTAL);
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_2, Names::Menu::Back);
    char str[14] = {};
    for (uint8_t idx_u8 = 0U; idx_u8 < (uint8_t)Menu::MENU_OUTLET_MAX; idx_u8++) {
        sprintf(str, "%s %d", Names::Menu::Outlet, idx_u8 + 1);
        Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_3 + idx_u8 * 8, str);
    }
}

static void setCursor(void) {
  switch(HmiData_s.Menu_s.Selected_e) {
    case Menu::OUTLET_MENU:             CLAMP(HmiData_s.Menu_s.SelectedIdx_u8, 0, Menu::MENU_OUTLET_MAX); break;
    case Menu::OVERVIEW_MENU:           HmiData_s.Menu_s.SelectedIdx_u8 = 0; break;
    case Menu::SETTINGS_MENU:           CLAMP(HmiData_s.Menu_s.SelectedIdx_u8, 0, Menu::MENU_SET_MAX);  break;
    case Menu::MAIN_MENU:               CLAMP(HmiData_s.Menu_s.SelectedIdx_u8, 0, Menu::MENU_MAIN_MAX-1); break;
    case Menu::OUTLET_DETAIL_1_MENU:
    case Menu::OUTLET_DETAIL_2_MENU:
    case Menu::OUTLET_DETAIL_3_MENU:
    case Menu::OUTLET_DETAIL_4_MENU:    CLAMP(HmiData_s.Menu_s.SelectedIdx_u8, 0, Menu::MENU_OUT_MAX); break;
    case Menu::SETTINGS_TIME_MENU:      CLAMP(HmiData_s.Menu_s.SelectedIdx_u8, 0, Menu::MENU_TIME_MAX); break;
    case Menu::SETTINGS_DISPLAY_MENU:   CLAMP(HmiData_s.Menu_s.SelectedIdx_u8, 0, Menu::MENU_DISPLAY_MAX); break;
    case Menu::SETTINGS_OVERALL_OUTLET_MENU:   CLAMP(HmiData_s.Menu_s.SelectedIdx_u8, 0, Menu::MENU_OVERALL_OUTLET_MAX); break;
    default: break;
  }
}

static void ShowCursor_Main(void) {
    for (uint8_t idx_u8 = 0; idx_u8 < 7; idx_u8++){
        if (HmiData_s.Menu_s.SelectedIdx_u8 + 0 == idx_u8) 
            Display::Print(0, idx_u8 * 8 + Display::LINE_2, Names::Menu::Arrow);
        else
            Display::Print(0, idx_u8 * 8 + Display::LINE_2, Names::Menu::Space);
    }
}

static void showOutletDetail(uint8_t out) {
    char str[20];
    sprintf(str, "%s%s%s %d", Names::Menu::Outlets, Names::Menu::Divide, Names::Menu::Outlet, out + 1);
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_1, str);
    Display::HLine(Display::LINE_HORIZONTAL);
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_2, Names::Menu::Back);
    showValue(HmiData_s.Menu_s.X_u8, Display::LINE_3, &InputData_p.Wb_p->Out_as[out][SETTING_OUT_AMOUNT]);
    showValue(HmiData_s.Menu_s.X_u8, Display::LINE_4, &InputData_p.Wb_p->Out_as[out][SETTING_OUT_CYCLE]);
    showValue(HmiData_s.Menu_s.X_u8, Display::LINE_5, &InputData_p.Wb_p->Out_as[out][SETTING_OUT_DAYTIME]);
    showValue(HmiData_s.Menu_s.X_u8, Display::LINE_6, &InputData_p.Wb_p->Out_as[out][SETTING_OUT_OFFSET]);
    showValue(HmiData_s.Menu_s.X_u8, Display::LINE_7, &InputData_p.Wb_p->Out_as[out][SETTING_OUT_TESTRUN]); 
}

static void showTimeDetail(void) {
  if (HmiData_s.EditMode_bo == false) { /* only update those values if no edit mode enabled */
    InputData_p.Wb_p->Time_s.Hour_s.Value_s16  = InputData_p.Time_p->hour;
    InputData_p.Wb_p->Time_s.Min_s.Value_s16   = InputData_p.Time_p->minute;
    InputData_p.Wb_p->Time_s.Year_s.Value_s16  = InputData_p.Time_p->year;
    InputData_p.Wb_p->Time_s.Month_s.Value_s16 = InputData_p.Time_p->month;
    InputData_p.Wb_p->Time_s.Day_s.Value_s16   = InputData_p.Time_p->day;
  }
  char str[20];
  sprintf(str, "%s%s%s", Names::Menu::Settings, Names::Menu::Divide, Names::Menu::Time);
  Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_1, str);
  Display::HLine(Display::LINE_HORIZONTAL);
  Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_2, Names::Menu::Back);
  showValue(HmiData_s.Menu_s.X_u8, Display::LINE_3, &InputData_p.Wb_p->Time_s.Hour_s);
  showValue(HmiData_s.Menu_s.X_u8, Display::LINE_4, &InputData_p.Wb_p->Time_s.Min_s);
  showValue(HmiData_s.Menu_s.X_u8, Display::LINE_5, &InputData_p.Wb_p->Time_s.Year_s);
  showValue(HmiData_s.Menu_s.X_u8, Display::LINE_6, &InputData_p.Wb_p->Time_s.Month_s);
  showValue(HmiData_s.Menu_s.X_u8, Display::LINE_7, &InputData_p.Wb_p->Time_s.Day_s);
}

static void showCurrentWateringStatus(void) {
    char str[20];
    uint8_t percent_u8 = 100.0 * ((float)(HmiData_s.CurrentAmount_u16 / (float)HmiData_s.TotalMilliliter_u16 + 0.001));
    Display::Print(0, 1, Names::Headline, 1, 0);
    sprintf(str, "%s %d: %3d %%", Names::Menu::Outlet, HmiData_s.CurrentOutlet_s8 + 1, percent_u8);
    Display::Print(0, 2, str, 1, 0);
    sprintf(str, "%4d/%4d ml", HmiData_s.TotalMilliliter_u16, HmiData_s.CurrentAmount_u16);
    Display::Print(0, 3, str, 1, 0);
}

static void showDisplayDetail(void) {
    char str[20];
    sprintf(str, "%s%s%s", Names::Menu::Settings, Names::Menu::Divide, Names::Menu::Display);
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_1, str);
    Display::HLine(Display::LINE_HORIZONTAL);
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_2, Names::Menu::Back);
    showValue(HmiData_s.Menu_s.X_u8, Display::LINE_3, &InputData_p.Wb_p->Display_s.ScreenSaver_s);
    showValue(HmiData_s.Menu_s.X_u8, Display::LINE_4, &InputData_p.Wb_p->Display_s.Sleep_s);
}

static void showOverallOutletDetail(void) {
    char str[20];
    sprintf(str, "%s%s%s", Names::Menu::Settings, Names::Menu::Divide, Names::Menu::Outlets);
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_1, str);
    Display::HLine(Display::LINE_HORIZONTAL);
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_2, Names::Menu::Back);
    showValue(HmiData_s.Menu_s.X_u8, Display::LINE_3, &InputData_p.Wb_p->Outlets_s.FlowAmount_s);
    showValue(HmiData_s.Menu_s.X_u8, Display::LINE_4, &InputData_p.Wb_p->Outlets_s.Enable_s);
}

static void showOverview(void) {
    CLAMP(HmiData_s.overviewNrDays_u8, OVERVIEW_MIN_DISPLAYED_DAYS, OVERVIEW_MAX_DISPLAYED_DAYS);
    uint8_t days_u8 = HmiData_s.overviewNrDays_u8;
    const uint8_t START_Y = 22;
    const uint8_t START_X = 12;
    uint8_t maxWidth_u8 = 128-START_X-1;
    uint8_t maxHeight_u8 = 64-START_Y-1;
    uint8_t widthDays_u8 = (float)maxWidth_u8/(float)days_u8;
    uint8_t heightOuts_u8 = (float)maxHeight_u8/(float)WATER_OUTLET_MAX;
    maxWidth_u8 = widthDays_u8 * days_u8; /* override existing width because of rounding errors!*/
    maxHeight_u8 = heightOuts_u8 * (days_u8 + 1);
    char buff[3];
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_1, Names::Menu::Overview);
    Display::HLine(Display::LINE_HORIZONTAL);
    Display::Print(HmiData_s.Menu_s.X_u8, Display::LINE_2, Names::Menu::Back);
  
    /* print vertical lines and days */
    for (uint8_t idxDays_u8 = 0; idxDays_u8 < (days_u8+1); idxDays_u8++) {
        Display::Line(START_X + idxDays_u8*widthDays_u8, START_Y, START_X + idxDays_u8*widthDays_u8, START_Y + maxHeight_u8);
    }
    /* print horizontal lines and outs */
    for (uint8_t idxOuts_u8 = 0; idxOuts_u8 < (WATER_OUTLET_MAX+1); idxOuts_u8++) {
        Display::Line(START_X, START_Y + idxOuts_u8*heightOuts_u8, START_X + maxWidth_u8, START_Y + idxOuts_u8*heightOuts_u8);
        if ((idxOuts_u8 + 1) < 5) {
            sprintf(buff, "O%d", idxOuts_u8+1);
            uint8_t offset = START_Y + (float)heightOuts_u8 - 1.0 + (float)idxOuts_u8*(float)heightOuts_u8;
            Display::PrintXY(0, offset, buff);
        }
    }

    for (uint8_t idxOuts_u8 = 0; idxOuts_u8 < WATER_OUTLET_MAX; idxOuts_u8++) {
        uint8_t volHeight_u8 = (float)InputData_p.Wb_p->Out_as[idxOuts_u8][SETTING_OUT_AMOUNT].Value_s16/1000.0 * heightOuts_u8;
        uint8_t offset_u8 = (float)widthDays_u8/24.0 * (float)InputData_p.Wb_p->Out_as[idxOuts_u8][SETTING_OUT_DAYTIME].Value_s16;
        int16_t cyc_s16 = (float)InputData_p.Wb_p->Out_as[idxOuts_u8][SETTING_OUT_CYCLE].Value_s16;
        for(int16_t idxCyc_s16 = 0; idxCyc_s16<(24*days_u8); idxCyc_s16++) {
            if(idxCyc_s16%cyc_s16 == 0) {
            uint8_t offDays = (float)idxCyc_s16/24.0 * (float)widthDays_u8;
            Display::Box(START_X + offset_u8 + offDays, 1 + START_Y + idxOuts_u8*heightOuts_u8, (float)widthDays_u8/5.0, volHeight_u8);
            }
        }
    }
}

void ScreenSaver_Main(TimeType* time_p) {
  char tempBuff[5];
  char totalBuff[11];
  dtostrf(time_p->temperature, 4, 1, tempBuff);
  sprintf(totalBuff, "%02d:%02d:%02d", time_p->hour, time_p->minute, time_p->second);
  Display::Print(0, 1, totalBuff, 1, 0);
  sprintf(totalBuff, "%02d.%02d.%04d", time_p->day, time_p->month, time_p->year + 2000);
  Display::Print(0, 2, totalBuff, 1, 0);
  sprintf(totalBuff, "%s C", tempBuff); 
  Display::Print(0, 3, totalBuff, 1, 0);
}

static void ShowMenu_Main(void) {
    switch(HmiData_s.Menu_s.Selected_e) {
    case Menu::OUTLET_MENU:     showOutletMenu(); break;// Names::OutletMenu, (uint8_t)Menu::MENU_OUTLET_MAX, Names::MainMenu[0]); break;
        case Menu::OVERVIEW_MENU:   showOverview(); break;
        case Menu::SETTINGS_MENU:   showSettingMenu(); break;// Names::SettingsMenu, (uint8_t)Menu::MENU_SET_MAX, Names::MainMenu[1]);  break;
        case Menu::MAIN_MENU:       showMainMenu(); break;// Names::MainMenu, (uint8_t)Menu::MENU_MAIN_MAX, Names::Headline, false); break;
        case Menu::OUTLET_DETAIL_1_MENU: showOutletDetail(WATER_OUTLET_1); break;
        case Menu::OUTLET_DETAIL_2_MENU: showOutletDetail(WATER_OUTLET_2); break;
        case Menu::OUTLET_DETAIL_3_MENU: showOutletDetail(WATER_OUTLET_3); break;
        case Menu::OUTLET_DETAIL_4_MENU: showOutletDetail(WATER_OUTLET_4); break;
        case Menu::SETTINGS_TIME_MENU: showTimeDetail(); break;
        case Menu::SETTINGS_DISPLAY_MENU: showDisplayDetail(); break;
        case Menu::SETTINGS_OVERALL_OUTLET_MENU: showOverallOutletDetail(); break;
        default: break;
    }
}

void ISR_A(void) {  
    HmiData_s.KeyPad_s.IsrValue_u8 = digitalRead(ENCODER_PIN_B) * 2 + digitalRead(ENCODER_PIN_A);
}

void ISR_PRESS(void) { 
    HmiData_s.KeyPad_s.IsrPress_bo = digitalRead(ENCODER_PIN_PRESS);
}

static void Pins_Init(void) {
  pinMode(ENCODER_PIN_A, INPUT_PULLUP); /* Configure as input, turn on pullup resistors */
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);
  pinMode(ENCODER_PIN_PRESS, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), ISR_A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_PRESS), ISR_PRESS, CHANGE);
  DEBUG("Pi");
}

static void KeyPad_Main() {
    static uint8_t lastVal = 3;
    static boolean lastPress = true;
    Key_EnumType state = KEY_NONE;
    if (lastVal == 1) {/* turns left */
        if (HmiData_s.KeyPad_s.IsrValue_u8 == 2) { /* a tick to the left is detected */
            state = KEY_DOWN;
        }
        else if (HmiData_s.KeyPad_s.IsrValue_u8 == 0 || HmiData_s.KeyPad_s.IsrValue_u8 == 3) {
            state = KEY_UP;
        }
    }
    else if (lastVal == 3) { /* turns right */
        if (HmiData_s.KeyPad_s.IsrValue_u8 == 0) { /* a tick to the right is detected */
            state = KEY_UP;
        }
        else if (HmiData_s.KeyPad_s.IsrValue_u8 == 2 || HmiData_s.KeyPad_s.IsrValue_u8 == 1) {
            state = KEY_DOWN;
        }
    }
    if (lastPress != HmiData_s.KeyPad_s.IsrPress_bo) {
        if (HmiData_s.KeyPad_s.IsrPress_bo == 0) {
            state = KEY_CENTER;
        }
        else {
            state = KEY_NONE;
        }
    }
    lastVal = HmiData_s.KeyPad_s.IsrValue_u8;
    lastPress = HmiData_s.KeyPad_s.IsrPress_bo;
    HmiData_s.KeyPad_s.Key_e = state;
}

void keyCenterEvent() {
  if (HmiData_s.KeyPad_s.Clear_bo == false) return;
  DEBUG("KC");
  DEBUG("M"); DEBUG_VALUE(HmiData_s.Menu_s.SelectedIdx_u8);
  uint8_t selOut;
  ValueType *editVal_p = NULL;

  switch(HmiData_s.Menu_s.Selected_e) {
    case Menu::MAIN_MENU:
      switch(HmiData_s.Menu_s.SelectedIdx_u8) {
        case Menu::MENU_MAIN_OUTPUT: HmiData_s.Menu_s.Selected_e = Menu::OUTLET_MENU; break;
        case Menu::MENU_MAIN_OVERVIEW: HmiData_s.Menu_s.Selected_e = Menu::OVERVIEW_MENU; HmiData_s.overviewNrDays_u8 = 3; break;
        case Menu::MENU_MAIN_SETTINGS: HmiData_s.Menu_s.Selected_e = Menu::SETTINGS_MENU; break;
        default: break;
      }
    break;
    case Menu::OUTLET_MENU:
      switch(HmiData_s.Menu_s.SelectedIdx_u8) {
        case 0: HmiData_s.Menu_s.Selected_e = Menu::MAIN_MENU; break;
        case Menu::MENU_OUTLET_1+1: HmiData_s.Menu_s.Selected_e = Menu::OUTLET_DETAIL_1_MENU; break;
        case Menu::MENU_OUTLET_2+1: HmiData_s.Menu_s.Selected_e = Menu::OUTLET_DETAIL_2_MENU; break;
        case Menu::MENU_OUTLET_3+1: HmiData_s.Menu_s.Selected_e = Menu::OUTLET_DETAIL_3_MENU; break;
        case Menu::MENU_OUTLET_4+1: HmiData_s.Menu_s.Selected_e = Menu::OUTLET_DETAIL_4_MENU; break;
      }
    break;
    case Menu::OVERVIEW_MENU:
      HmiData_s.Menu_s.Selected_e = Menu::MAIN_MENU; 
    break;
    case Menu::SETTINGS_MENU:
      switch(HmiData_s.Menu_s.SelectedIdx_u8) {
        case 0: HmiData_s.Menu_s.Selected_e = Menu::MAIN_MENU; break;
        case Menu::MENU_SET_TIME_DATE+1: HmiData_s.Menu_s.Selected_e = Menu::SETTINGS_TIME_MENU; break;
        case Menu::MENU_SET_DISPLAY + 1: HmiData_s.Menu_s.Selected_e = Menu::SETTINGS_DISPLAY_MENU; break;
        case Menu::MENU_SET_OVERALL_OUTLETS + 1: HmiData_s.Menu_s.Selected_e = Menu::SETTINGS_OVERALL_OUTLET_MENU; break;
      }
    break;
    case Menu::OUTLET_DETAIL_1_MENU:
    case Menu::OUTLET_DETAIL_2_MENU:
    case Menu::OUTLET_DETAIL_3_MENU:
    case Menu::OUTLET_DETAIL_4_MENU: 
      if (HmiData_s.Menu_s.Selected_e == Menu::OUTLET_DETAIL_1_MENU) selOut = WATER_OUTLET_1;
      if (HmiData_s.Menu_s.Selected_e == Menu::OUTLET_DETAIL_2_MENU) selOut = WATER_OUTLET_2;
      if (HmiData_s.Menu_s.Selected_e == Menu::OUTLET_DETAIL_3_MENU) selOut = WATER_OUTLET_3;
      if (HmiData_s.Menu_s.Selected_e == Menu::OUTLET_DETAIL_4_MENU) selOut = WATER_OUTLET_4;
      
      switch(HmiData_s.Menu_s.SelectedIdx_u8) {
        case 0: HmiData_s.Menu_s.Selected_e = Menu::OUTLET_MENU; break;
        case Menu::MENU_OUT_AMOUNT+1:  editVal_p = &InputData_p.Wb_p->Out_as[selOut][SETTING_OUT_AMOUNT]; break;
        case Menu::MENU_OUT_CYCLE+1:   editVal_p = &InputData_p.Wb_p->Out_as[selOut][SETTING_OUT_CYCLE]; break;
        case Menu::MENU_OUT_DAYTIME+1: editVal_p = &InputData_p.Wb_p->Out_as[selOut][SETTING_OUT_DAYTIME]; break;
        case Menu::MENU_OUT_OFFSET+1:  editVal_p = &InputData_p.Wb_p->Out_as[selOut][SETTING_OUT_OFFSET]; break;
        case Menu::MENU_OUT_TESTRUN+1: editVal_p = &InputData_p.Wb_p->Out_as[selOut][SETTING_OUT_TESTRUN]; break;
      }
    break;
    case Menu::SETTINGS_TIME_MENU:
      switch(HmiData_s.Menu_s.SelectedIdx_u8) {
        case 0: HmiData_s.Menu_s.Selected_e = Menu::SETTINGS_MENU; break;
        case Menu::MENU_TIME_HOUR+1:     editVal_p = &InputData_p.Wb_p->Time_s.Hour_s; break;
        case Menu::MENU_TIME_MINUTE+1:   editVal_p = &InputData_p.Wb_p->Time_s.Min_s; break;
        case Menu::MENU_TIME_YEAR + 1:   editVal_p = &InputData_p.Wb_p->Time_s.Year_s; break;
        case Menu::MENU_TIME_MONTH+1:    editVal_p = &InputData_p.Wb_p->Time_s.Month_s; break;
        case Menu::MENU_TIME_DAY+1:      editVal_p = &InputData_p.Wb_p->Time_s.Day_s; break;
      }
    break;
    case Menu::SETTINGS_DISPLAY_MENU: 
        switch(HmiData_s.Menu_s.SelectedIdx_u8) {
            case 0: HmiData_s.Menu_s.Selected_e = Menu::SETTINGS_MENU; break;
            case Menu::MENU_DISPLAY_SCREENSAVER+1: editVal_p = &InputData_p.Wb_p->Display_s.ScreenSaver_s; break;
            case Menu::MENU_DISPLAY_STANDBY+1:     editVal_p = &InputData_p.Wb_p->Display_s.Sleep_s; break;
        }
    break;
    case Menu::SETTINGS_OVERALL_OUTLET_MENU:
        switch (HmiData_s.Menu_s.SelectedIdx_u8) {
            case 0: HmiData_s.Menu_s.Selected_e = Menu::SETTINGS_MENU; break;
            case Menu::MENU_OVERALL_OUTLET_FLOW_AMOUNT + 1: editVal_p = &InputData_p.Wb_p->Outlets_s.FlowAmount_s; break;
            case Menu::MENU_OVERALL_OUTLET_ENABLE + 1:      editVal_p = &InputData_p.Wb_p->Outlets_s.Enable_s; break;
        }
    break;
    default: break;
  }
  
  if (NULL != editVal_p) {
      enableEditMode(editVal_p);
  } else {
      HmiData_s.Menu_s.SelectedIdx_u8 = 0;
  }

  HmiData_s.KeyPad_s.Clear_bo = false;
  DEBUG("M"); DEBUG_VALUE((uint8_t)HmiData_s.Menu_s.Selected_e);
}

void enableEditMode(ValueType* val) {
  static int32_t lastValue = val->Value_s16;
  if (HmiData_s.EditMode_bo == true) {
    HmiData_s.EditMode_bo = false;
    if (lastValue != val->Value_s16) { /* save to eeprom if it has been changed */
        DEBUG("NVMS"); DEBUG_VALUE(val->Value_s16);
      NVM::SetValue(val);
    }
  } else {
    HmiData_s.EditMode_bo = true; 
    HmiData_s.CurrentValue_p = val;
    lastValue = val->Value_s16;
  }
}

void keyDownEvent() {  
  if (HmiData_s.KeyPad_s.Clear_bo == false) return; 
  DEBUG("KD");
  if(HmiData_s.EditMode_bo == true) {
    HmiData_s.CurrentValue_p->Value_s16 += HmiData_s.CurrentValue_p->Step_s16;
    CLAMP(HmiData_s.CurrentValue_p->Value_s16, HmiData_s.CurrentValue_p->Min_s16, HmiData_s.CurrentValue_p->Max_s16);
  } else {
    HmiData_s.Menu_s.SelectedIdx_u8++; 
    HmiData_s.overviewNrDays_u8++;
  }
  HmiData_s.KeyPad_s.Clear_bo = false;
}

void keyUpEvent() {
  if (HmiData_s.KeyPad_s.Clear_bo == false) return;
  DEBUG("KU");
  if(HmiData_s.EditMode_bo == true) {
    HmiData_s.CurrentValue_p->Value_s16 -= HmiData_s.CurrentValue_p->Step_s16;
    CLAMP(HmiData_s.CurrentValue_p->Value_s16, HmiData_s.CurrentValue_p->Min_s16, HmiData_s.CurrentValue_p->Max_s16);
  } else {
    if (HmiData_s.Menu_s.SelectedIdx_u8 > 0) {
      HmiData_s.Menu_s.SelectedIdx_u8--;
    }
    HmiData_s.overviewNrDays_u8--;
  }
  HmiData_s.KeyPad_s.Clear_bo = false;
}

void keyReleasedEvent() {
    if (HmiData_s.KeyPad_s.Clear_bo == false) {
        DEBUG("KR");
        HmiData_s.KeyPad_s.Clear_bo = true;
    }
}

void showValue(uint8_t posX, uint8_t posY, ValueType* value) {
    char buff[16];
    sprintf(buff, "%s:", value->Name_pc);
    Display::Print(posX, posY, value->Name_pc);
    sprintf(buff, "%4d", value->Value_s16);
    Display::Print(posX + DISPLAY_VALUE_POSITION, posY, buff);
    Display::Print(posX + DISPLAY_UNIT_POSITION, posY, value->Unit_pc);
}

static void editValue(void) {
    static uint32_t lastBlink = millis();
    static boolean toggleValue = false; 
  
    if(HmiData_s.EditMode_bo == true) {
        char buff[5];
        snprintf(buff, 5, "%4d", HmiData_s.CurrentValue_p->Value_s16);
        if (millis() - lastBlink >= EDIT_MODE_BLINK_DURATION) {
            toggleValue = !toggleValue;
            lastBlink = millis();
        } 
        if (toggleValue == true) {
            Display::Print(HmiData_s.Menu_s.X_u8 + DISPLAY_VALUE_POSITION, HmiData_s.Menu_s.SelectedIdx_u8 * 8 + Display::LINE_2, buff, 0, 1); /* highlight text */
        } else {
            Display::Print(HmiData_s.Menu_s.X_u8 + DISPLAY_VALUE_POSITION, HmiData_s.Menu_s.SelectedIdx_u8 * 8 + Display::LINE_2, buff);
        }
    } else if (toggleValue == true) { /* if edit mode released but text highlight still active deactivate here */
        toggleValue = false;
    }
}

/*************************************************/
/*         GLOBAL FUNCTION DEFINITIONS           */
/*************************************************/
void Display_Main(DISPLAY_MODE displayMode_e) {
    HmiData_s.DisplayMode_e = displayMode_e;
    static boolean standbyActive_bo = false;
    switch (HmiData_s.DisplayMode_e) {
    case DISPLAY_NORMAL: 
        switch (HmiData_s.KeyPad_s.Key_e) {
            case KEY_CENTER:  keyCenterEvent(); break;
            case KEY_DOWN:    keyDownEvent(); break;
            case KEY_UP:      keyUpEvent(); break;
            case KEY_NONE:
            default:          keyReleasedEvent(); break;
        }
        setCursor();
        ShowCursor_Main();
        ShowMenu_Main();
        editValue();
        break;

    case DISPLAY_SCREENSAVER:
        DEBUG("DSa");
        HmiData_s.EditMode_bo = false;
        ScreenSaver_Main(InputData_p.Time_p); 
    break;
    case DISPLAY_STANDBY: 
        DEBUG("DSb");
        HmiData_s.EditMode_bo = false;
        Display::Sleep();
        standbyActive_bo = true;
    break;
    case DISPLAY_INIT:
        DEBUG("Di");
        Display::Awake();
        standbyActive_bo = false;
        break;
    case DISPLAY_WATERING:
        DEBUG("Dw");
        if (true == standbyActive_bo) {
            Display::Awake();
            standbyActive_bo = false;
        }
        showCurrentWateringStatus();
        break;
    }

    KeyPad_Main(); /* get key in the display loop otherwise it ll stuck */
}

void Display_Init(WateringBoy_DataType *wb, TimeType* time_p) {
    DEBUG("DI");
    Display::Init();
    HmiData_s.Menu_s.SelectedIdx_u8 = Menu::MENU_MAIN_OUTPUT;
    HmiData_s.Menu_s.Selected_e = Menu::MAIN_MENU;
    HmiData_s.KeyPad_s.Key_e = KEY_NONE;
    InputData_p.Time_p = time_p;
    InputData_p.Wb_p = wb;

    showMainMenu();

    Pins_Init();
}

void DisplayUpdateWatering(int8_t currentOutlet, uint16_t totalMilliliter_u16, uint16_t currentAmount_u16)
{
    DEBUG("DW");
    HmiData_s.DisplayMode_e = DISPLAY_WATERING;
    HmiData_s.CurrentOutlet_s8 = currentOutlet;
    HmiData_s.TotalMilliliter_u16 = totalMilliliter_u16;
    HmiData_s.CurrentAmount_u16 = currentAmount_u16;
}