#include "types.h"
#include "displayWrapper.h"
#include "names.h"
#include "utils.h"
#include "NVM.h"

typedef enum Key_EnumTag { KEY_LEFT, KEY_CENTER, KEY_DOWN, KEY_RIGHT, KEY_UP, KEY_NONE } Key_EnumType;

typedef struct Display_Tag {
  const uint8_t menuX_u8 = 8;
  const uint8_t menuY_u8 = 8;
  const uint8_t menuVal_u8 = 60;
  const uint8_t menuUnit_u8 = 88;
  uint8_t selectedMenuIdx_u8;
  Menu::Menu_Enum selectedMenu; 
  boolean ClearFlag_bo;
  boolean EditMode_bo;
  TimeType* Time_p;
  ValueType* CurrentValue_p;
  Key_EnumType* Key_p;
} Display_Type;

/* local variables */
static Display_Type DispData_s;
static uint8_t overviewDaysCount = 3;
/* local function declarations */
static void readKey(Key_EnumType& key);
static void setCursor(void);
static void showMenu(const char menuEntries[][CHARS_MAX], uint8_t totalEntries, const char header[], boolean subMenu = true);
static void showCursor(void);
static void showOverview(void);
static void showHeader(const char header[]);
static void showValue(uint8_t posX, uint8_t posY, ValueType* value);
static void showDisplayDetail(void);
static void showTimeDetail(void);
static void showOutletDetail(uint8_t out);
static void menuUpdate(void);
static void enableEditMode(ValueType* val);
static void initPins();
static Key_EnumType ReadEncoder(void);

/* global function definitions */
void initDisplay(void);
void runDisplay(Key_EnumType* key, TimeType* time);
void showScreenSaver(TimeType& t);

/* local function definitions */
static void showHeader(const char header[]) {
  Display::Clear();
  Display::Print(DispData_s.menuX_u8, 1, header); /* "WATERING BOT" */
}

static void showMenu(const char menuEntries[][CHARS_MAX], uint8_t totalEntries, const char header[], boolean subMenu) {
  showHeader(header);
  if (subMenu == true) {
    Display::Print(DispData_s.menuX_u8, 2, Names::Back);
    for (uint8_t idx = 0; idx < totalEntries; idx++){
      Display::Print(DispData_s.menuX_u8, idx + 3, menuEntries[idx]);
    }
  } else {
    for (uint8_t idx = 0; idx < totalEntries; idx++){
      Display::Print(DispData_s.menuX_u8, idx + 2, menuEntries[idx]);
    }
  }
}

static void setCursor(void) {
  switch(DispData_s.selectedMenu) {
    case Menu::OUTLET_MENU:         DispData_s.selectedMenuIdx_u8 = constrain(DispData_s.selectedMenuIdx_u8, 0, Menu::MENU_OUTLET_MAX); break;
    case Menu::OVERVIEW_MENU:       DispData_s.selectedMenuIdx_u8 = 0; break;
    case Menu::SETTINGS_MENU:       DispData_s.selectedMenuIdx_u8 = constrain(DispData_s.selectedMenuIdx_u8, 0, Menu::MENU_SET_MAX);  break;
    case Menu::MAIN_MENU:           DispData_s.selectedMenuIdx_u8 = constrain(DispData_s.selectedMenuIdx_u8, 0, Menu::MENU_MAIN_MAX-1); break;
    case Menu::OUTLET_DETAIL_1_MENU:
    case Menu::OUTLET_DETAIL_2_MENU:
    case Menu::OUTLET_DETAIL_3_MENU:
    case Menu::OUTLET_DETAIL_4_MENU: DispData_s.selectedMenuIdx_u8 = constrain(DispData_s.selectedMenuIdx_u8, 0, Menu::MENU_OUT_MAX); break;
    case Menu::SETTINGS_TIME_MENU:  DispData_s.selectedMenuIdx_u8 = constrain(DispData_s.selectedMenuIdx_u8, 0, Menu::MENU_TIME_MAX); break;
    case Menu::SETTINGS_DISPLAY_MENU: DispData_s.selectedMenuIdx_u8 = constrain(DispData_s.selectedMenuIdx_u8, 0, 1); break;
    default: break;
  }
}

static void showCursor(void) {
  for (uint8_t idx = 2; idx <= 8; idx++){
    if (DispData_s.selectedMenuIdx_u8 + 2 == idx) 
      Display::Print(0, idx, Names::Arrow);
    else
      Display::Print(0, idx, Names::Space);
  }
}

static void showOutletDetail(uint8_t out) {
    char str[16];
    strcpy(str, Names::MainMenu[0]);
    strcat(str, Names::Divide);
    strncat(str, Names::OutletMenu[0], 6);
    showHeader(str);
    Display::Print(DispData_s.menuX_u8, 2, Names::Back);
    showValue(DispData_s.menuX_u8, Display::LINE_3, &WB.out[out][SETTING_OUT_AMOUNT]);
    showValue(DispData_s.menuX_u8, Display::LINE_4, &WB.out[out][SETTING_OUT_CYCLE]);
    showValue(DispData_s.menuX_u8, Display::LINE_5, &WB.out[out][SETTING_OUT_DAYTIME]);
    showValue(DispData_s.menuX_u8, Display::LINE_6, &WB.out[out][SETTING_OUT_OFFSET]);
    showValue(DispData_s.menuX_u8, Display::LINE_7, &WB.out[out][SETTING_OUT_TESTRUN]);  
}

static void showTimeDetail(void) {
  if (DispData_s.EditMode_bo == false) { /* only update those values if no edit mode enabled */
    WB.time.hour.value  = DispData_s.Time_p->hour;
    WB.time.min.value   = DispData_s.Time_p->minute;
    WB.time.year.value  = DispData_s.Time_p->year;
    WB.time.month.value = DispData_s.Time_p->month;
    WB.time.day.value   = DispData_s.Time_p->day;
  }
  char str[16];
  strcpy(str, Names::MainMenu[2]);
  strcat(str, Names::Divide);
  strcat(str, Names::SettingsMenu[0]);
  showHeader(str);

  Display::Print(DispData_s.menuX_u8, Display::LINE_2, Names::Back);
  showValue(DispData_s.menuX_u8, Display::LINE_3, &WB.time.hour);
  showValue(DispData_s.menuX_u8, Display::LINE_4, &WB.time.min);
  showValue(DispData_s.menuX_u8, Display::LINE_5, &WB.time.year);
  showValue(DispData_s.menuX_u8, Display::LINE_6, &WB.time.month);
  showValue(DispData_s.menuX_u8, Display::LINE_7, &WB.time.day);
}

static void showDisplayDetail(void) {
    char str[16];
    strcpy(str, Names::MainMenu[2]);
    strcat(str, Names::Divide);
    strcat(str, Names::SettingsMenu[1]);
    showHeader(str);

    Display::Print(DispData_s.menuX_u8, Display::LINE_2, Names::Back);
    Display::Print(DispData_s.menuX_u8, Display::LINE_3, Names::SettingsMenu[1]);
}

static void showOverview(void) {
  overviewDaysCount = constrain(overviewDaysCount, 2, 10);
  uint8_t days = overviewDaysCount;
  uint8_t outs = 4;
  //uint8_t margin = 10;
  uint8_t startY = 18;
  uint8_t startX = 12;
  uint8_t maxWidth = 128-startX-1;
  uint8_t maxHeight = 64-startY-1;
  uint8_t widthDays = (float)maxWidth/(float)days;
  uint8_t heightOuts = (float)maxHeight/(float)outs;
  maxWidth = widthDays * days; /* override existing width because of rounding errors!*/
  maxHeight = heightOuts * (days + 1);
  char buff[5];
  showHeader(Names::MainMenu[1]);
  Display::Print(DispData_s.menuX_u8, Display::LINE_2, Names::Back);
  
  /* print vertical lines and days */
  for (uint8_t idxDays = 0; idxDays < (days+1); idxDays++) {
    Display::Line(startX + idxDays*widthDays, startY, startX + idxDays*widthDays, startY + maxHeight);
  }
  /* print horizontal lines and outs */
  for (uint8_t idxOuts = 0; idxOuts < (outs+1); idxOuts++) {
    Display::Line(startX, startY + idxOuts*heightOuts, startX + maxWidth, startY + idxOuts*heightOuts);
    if ((idxOuts + 1) < 5) {
      sprintf(buff, "O%d", idxOuts+1);
      buff[4] = '\0';
      uint8_t offset = startY + (float)heightOuts - 1.0 + (float)idxOuts*(float)heightOuts;
      Display::PrintXY(0, offset, buff);
    }
  }

  for (uint8_t idxOuts = 0; idxOuts < outs; idxOuts++) {
    uint8_t volHeight = (float)WB.out[idxOuts][SETTING_OUT_AMOUNT].value/1000.0 * heightOuts;
    uint8_t offset = (float)widthDays/24.0 * (float)WB.out[idxOuts][SETTING_OUT_DAYTIME].value;
    uint16_t cyc = (float)WB.out[idxOuts][SETTING_OUT_CYCLE].value;
    for(uint16_t idxCyc = 0; idxCyc<(24*days); idxCyc++) {
      if(idxCyc%cyc == 0) {
        uint8_t offDays = (float)idxCyc/24.0 * (float)widthDays;
        Display::Box(startX + offset + offDays, 1 + startY + idxOuts*heightOuts, (float)widthDays/5.0, volHeight);
      }
    }
  }
}

void showScreenSaver(TimeType& t) {
  char tempBuff[5];
  char totalBuff[11];
  dtostrf(t.temperature, 4, 1, tempBuff);
  sprintf(totalBuff, "%02d:%02d:%02d", t.hour, t.minute, t.second);
  Display::Print(0, 1, totalBuff, 1, 0);
  sprintf(totalBuff, "%02d.%02d.%04d", t.day, t.month, t.year + 2000); 
  Display::Print(0, 2, totalBuff, 1, 0);
  sprintf(totalBuff, "%s C", tempBuff); 
  Display::Print(0, 3, totalBuff, 1, 0);
}

static void menuUpdate(void) {
    switch(DispData_s.selectedMenu) {
        case Menu::OUTLET_MENU:     showMenu(Names::OutletMenu,  (uint8_t)Menu::MENU_OUTLET_MAX, Names::MainMenu[0]); break;
        case Menu::OVERVIEW_MENU:   showOverview(); break;
        case Menu::SETTINGS_MENU:   showMenu(Names::SettingsMenu, (uint8_t)Menu::MENU_SET_MAX, Names::MainMenu[1]);  break;
        case Menu::MAIN_MENU:       showMenu(Names::MainMenu, (uint8_t)Menu::MENU_MAIN_MAX, Names::Headline, false); break;
        case Menu::OUTLET_DETAIL_1_MENU: showOutletDetail(WATER_OUTLET_1); break;
        case Menu::OUTLET_DETAIL_2_MENU: showOutletDetail(WATER_OUTLET_2); break;
        case Menu::OUTLET_DETAIL_3_MENU: showOutletDetail(WATER_OUTLET_3); break;
        case Menu::OUTLET_DETAIL_4_MENU: showOutletDetail(WATER_OUTLET_4); break;
        case Menu::SETTINGS_TIME_MENU: showTimeDetail(); break;
        case Menu::SETTINGS_DISPLAY_MENU: showDisplayDetail(); break;
        default: break;
    }
}

void initDisplay() {
  Display::Init();
  DispData_s.selectedMenuIdx_u8 = Menu::MENU_MAIN_OUTPUT;
  DispData_s.selectedMenu = Menu::MAIN_MENU;
  *DispData_s.Key_p = KEY_NONE;
 
  showMenu(Names::MainMenu, (uint8_t)Menu::MENU_MAIN_MAX, Names::Headline, false);

  initPins();
}

enum enDigitalPins {
    dpInEncoderA=3,
    dpInEncoderB=4,
    dpInEncoderPress=2,
};
volatile uint8_t ISRValue;
volatile bool ISRPress;
void ISR_A() {  ISRValue = digitalRead(dpInEncoderB) * 2 + digitalRead(dpInEncoderA); }
void ISR_PRESS() { ISRPress = digitalRead(dpInEncoderPress); }

static void initPins(void) {
  // Rotary encoder input lines
  // Configure as input, turn on pullup resistors
  pinMode(dpInEncoderA, INPUT_PULLUP);
  //digitalWrite(dpInEncoderA, HIGH);
  pinMode(dpInEncoderB, INPUT_PULLUP);
  //digitalWrite(dpInEncoderB, HIGH);
  pinMode(dpInEncoderPress, INPUT_PULLUP);
  //digitalWrite(dpInEncoderPress, HIGH);

  attachInterrupt(digitalPinToInterrupt(dpInEncoderA), ISR_A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(dpInEncoderPress), ISR_PRESS, CHANGE);
  DEBUG("init pins", 0);
}

static Key_EnumType ReadEncoder(void) {
  static uint8_t lastVal = 3;
  static uint8_t lastPress = 1;
  Key_EnumType state = KEY_NONE;
  if (lastVal == 1) {/* turns left */ 
    if (ISRValue == 2) { /* a tick to the left is detected */
      state = KEY_DOWN;
      //Serial.println("DOWN");
    } else if (ISRValue == 0 || ISRValue == 3) {
      //Serial.println("UP");
      state = KEY_UP;
    }
  } else if (lastVal == 3) { /* turns right */
    if (ISRValue == 0) { /* a tick to the right is detected */
      //Serial.println("UP");
      state = KEY_UP;
    } else if (ISRValue == 2 || ISRValue == 1) {
      //Serial.println("DOWN");
      state = KEY_DOWN;
    }
  }
  if (lastPress != ISRPress) {
    if (ISRPress == 0) {
    //Serial.println("CENTER");
      state = KEY_CENTER;
    } else {
      state = KEY_NONE;
    }
  } else {
    //Serial.println("-");
  }
  lastVal = ISRValue;
  lastPress = ISRPress;
  
  return state;
}

void readKey(Key_EnumType& key) {
  /*
  static unsigned long debounceTime = 100;
  static unsigned long lastDebounceTime = 0;
  static Key_EnumType lastButtonState = KEY_NONE;
  static Key_EnumType buttonState = KEY_NONE;
  
  Key_EnumType currentState = (Key_EnumType)((float)analogRead(0) / 128.0 + 0.5);
  */
  /*static Key_EnumType buttonState = KEY_NONE;
  if (Serial.available() > 0) {
    int inByte = Serial.read();
    switch (inByte) {
      case 'c':
        buttonState = KEY_CENTER;
        break;
      case 'd':
        buttonState = KEY_DOWN;
        break;
      case 'u':
        buttonState = KEY_UP;
        break;
      case 'r':
        buttonState = KEY_NONE;
        break;
      default:
      buttonState = KEY_NONE;
    }
  }
  */
    /*
  if (currentState != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceTime) {
    if (currentState != buttonState) {
      buttonState = currentState;
    }
  }
  lastButtonState = currentState;
  */
  key = ReadEncoder();
}
void keyCenterEvent() {
  if (DispData_s.ClearFlag_bo == false) return;
  DEBUG("KEY_C", 0);
  DEBUG("MenuIdx: ", DispData_s.selectedMenuIdx_u8);
  uint8_t selOut = WATER_OUTLET_1;
  switch(DispData_s.selectedMenu) {
    case Menu::MAIN_MENU:
      switch(DispData_s.selectedMenuIdx_u8) {
        case Menu::MENU_MAIN_OUTPUT: DispData_s.selectedMenu = Menu::OUTLET_MENU; break;
        case Menu::MENU_MAIN_OVERVIEW: DispData_s.selectedMenu = Menu::OVERVIEW_MENU; overviewDaysCount = 3; break;
        case Menu::MENU_MAIN_SETTINGS: DispData_s.selectedMenu = Menu::SETTINGS_MENU; break;
        default: break;
      }
    break;
    case Menu::OUTLET_MENU:
      switch(DispData_s.selectedMenuIdx_u8) {
        case 0: DispData_s.selectedMenu = Menu::MAIN_MENU; break;
        case Menu::MENU_OUTLET_1+1: DispData_s.selectedMenu = Menu::OUTLET_DETAIL_1_MENU; break;
        case Menu::MENU_OUTLET_2+1: DispData_s.selectedMenu = Menu::OUTLET_DETAIL_2_MENU; break;
        case Menu::MENU_OUTLET_3+1: DispData_s.selectedMenu = Menu::OUTLET_DETAIL_3_MENU; break;
        case Menu::MENU_OUTLET_4+1: DispData_s.selectedMenu = Menu::OUTLET_DETAIL_4_MENU; break;
      }
    break;
    case Menu::OVERVIEW_MENU:
      DispData_s.selectedMenu = Menu::MAIN_MENU; 
    break;
    case Menu::SETTINGS_MENU:
      switch(DispData_s.selectedMenuIdx_u8) {
        case 0: DispData_s.selectedMenu = Menu::MAIN_MENU; break;
        case Menu::MENU_SET_TIME_DATE+1: DispData_s.selectedMenu = Menu::SETTINGS_TIME_MENU; break;
        case Menu::MENU_SET_DISPLAY+1: DispData_s.selectedMenu = Menu::SETTINGS_DISPLAY_MENU; break;
      }
    break;
    case Menu::OUTLET_DETAIL_1_MENU:
    case Menu::OUTLET_DETAIL_2_MENU:
    case Menu::OUTLET_DETAIL_3_MENU:
    case Menu::OUTLET_DETAIL_4_MENU: 
      if (DispData_s.selectedMenu == Menu::OUTLET_DETAIL_1_MENU) selOut = WATER_OUTLET_1;
      if (DispData_s.selectedMenu == Menu::OUTLET_DETAIL_2_MENU) selOut = WATER_OUTLET_2;
      if (DispData_s.selectedMenu == Menu::OUTLET_DETAIL_3_MENU) selOut = WATER_OUTLET_3;
      if (DispData_s.selectedMenu == Menu::OUTLET_DETAIL_4_MENU) selOut = WATER_OUTLET_4;
      
      switch(DispData_s.selectedMenuIdx_u8) {
        case 0: DispData_s.selectedMenu = Menu::OUTLET_MENU; break;
        case Menu::MENU_OUT_AMOUNT+1:  enableEditMode(&WB.out[selOut][SETTING_OUT_AMOUNT]); break;
        case Menu::MENU_OUT_CYCLE+1:   enableEditMode(&WB.out[selOut][SETTING_OUT_CYCLE]); break;
        case Menu::MENU_OUT_DAYTIME+1: enableEditMode(&WB.out[selOut][SETTING_OUT_DAYTIME]); break;
        case Menu::MENU_OUT_OFFSET+1:  enableEditMode(&WB.out[selOut][SETTING_OUT_OFFSET]); break;
        case Menu::MENU_OUT_TESTRUN+1: enableEditMode(&WB.out[selOut][SETTING_OUT_TESTRUN]); break;
      }
    break;
    case Menu::SETTINGS_TIME_MENU:
      switch(DispData_s.selectedMenuIdx_u8) {
        case 0: DispData_s.selectedMenu = Menu::SETTINGS_MENU; break;
        case Menu::MENU_TIME_HOUR+1:     enableEditMode(&WB.time.hour); break;
        case Menu::MENU_TIME_MINUTE+1:   enableEditMode(&WB.time.min); break;
        case Menu::MENU_TIME_YEAR+1:     enableEditMode(&WB.time.year); break;
        case Menu::MENU_TIME_MONTH+1:    enableEditMode(&WB.time.month); break;
        case Menu::MENU_TIME_DAY+1:      enableEditMode(&WB.time.day); break;
      }
    break;
    case Menu::SETTINGS_DISPLAY_MENU: 
      switch(DispData_s.selectedMenuIdx_u8) {
        case 0: DispData_s.selectedMenu = Menu::SETTINGS_MENU; break;
      }
    break;
    default: break;
  }
  DispData_s.ClearFlag_bo = false;
  if (   DispData_s.selectedMenu == Menu::MAIN_MENU 
      || DispData_s.selectedMenu == Menu::OUTLET_MENU 
      || DispData_s.selectedMenu == Menu::OVERVIEW_MENU 
      || DispData_s.selectedMenu == Menu::SETTINGS_MENU) {
    DispData_s.selectedMenuIdx_u8 = 0;      
  }
}

void enableEditMode(ValueType* val) {
  static int32_t lastValue = val->value;
  if (DispData_s.EditMode_bo == true) {
    DispData_s.EditMode_bo = false;
    if (lastValue != val->value) { /* save to eeprom if it has been changed */
      DEBUG("NVM saved: ", val->value);
      NVM::saveValue(val);
    }
  } else {
    DispData_s.EditMode_bo = true; 
    DispData_s.CurrentValue_p = val;
    lastValue = val->value;
  }
}

void keyDownEvent() {  
  if (DispData_s.ClearFlag_bo == false) return; 
  DEBUG("KEY_D", 0);
  if(DispData_s.EditMode_bo == true) {
    DispData_s.CurrentValue_p->value -= DispData_s.CurrentValue_p->step;
    DispData_s.CurrentValue_p->value = constrain(DispData_s.CurrentValue_p->value, DispData_s.CurrentValue_p->min, DispData_s.CurrentValue_p->max);
  } else {
    DispData_s.selectedMenuIdx_u8++; 
    overviewDaysCount++;
  }
  DispData_s.ClearFlag_bo = false;
}

void keyUpEvent() {
  if (DispData_s.ClearFlag_bo == false) return;
  DEBUG("KEY_U", 0);
  if(DispData_s.EditMode_bo == true) {
    DispData_s.CurrentValue_p->value += DispData_s.CurrentValue_p->step;
    DispData_s.CurrentValue_p->value = constrain(DispData_s.CurrentValue_p->value, DispData_s.CurrentValue_p->min, DispData_s.CurrentValue_p->max);
  } else {
    if (DispData_s.selectedMenuIdx_u8 > 0) {
      DispData_s.selectedMenuIdx_u8--;
    }
    overviewDaysCount--;
  }
  DispData_s.ClearFlag_bo = false;
}

void keyReleasedEvent() {
  if (DispData_s.ClearFlag_bo == false) {
    DEBUG("KEY_R", 0);
    DispData_s.ClearFlag_bo = true;
  }
}

//void keyLeftEvent() {
//  if (Disp.ClearFlag_b == false) return;
//  if (Disp.EditMode_b == true) return;
//  
//  DEBUG("KEY_L", 0);
//  switch(Disp.selectedMenu) {
//    case Menu::OUTLET_MENU: Disp.selectedMenu = Menu::MAIN_MENU; break;
//    case Menu::OVERVIEW_MENU: Disp.selectedMenu = Menu::MAIN_MENU; break;
//    case Menu::SETTINGS_MENU: Disp.selectedMenu = Menu::MAIN_MENU; break;
//    case Menu::OUTLET_DETAIL_1_MENU:
//    case Menu::OUTLET_DETAIL_2_MENU:
//    case Menu::OUTLET_DETAIL_3_MENU:
//    case Menu::OUTLET_DETAIL_4_MENU: Disp.selectedMenu = Menu::OUTLET_MENU; break;
//    case Menu::SETTINGS_TIME_MENU: Disp.selectedMenu = Menu::SETTINGS_MENU; break;
//    case Menu::SETTINGS_DISPLAY_MENU: Disp.selectedMenu = Menu::SETTINGS_MENU; break;
//    case Menu::MAIN_MENU: break;
//    default: break;
//  }
//  //Disp.selectedMenuIdx = 0;
//  Disp.ClearFlag_b = false;
//}

void showValue(uint8_t posX, uint8_t posY, ValueType* value) {
  char buff[CHARS_MAX] = { 0 };
  /* display name*/
  sprintf(buff, "%s:", value->name);
  Display::Print(posX, posY, value->name);
  /* display value */
  sprintf(buff, "%4d", value->value);
  Display::Print(posX + DispData_s.menuVal_u8, posY, buff);
  /* display value */
  Display::Print(posX + DispData_s.menuUnit_u8, posY, value->unit);
}

void editValue() {
  const uint32_t DURATION = 400;
  static uint32_t lastBlink = millis();
  static boolean toggleValue = false; 
  
  if(DispData_s.EditMode_bo == true) {
    char buff[5];
    snprintf(buff, 5, "%4d", DispData_s.CurrentValue_p->value);
    if (millis() - lastBlink >= DURATION) {
      toggleValue = !toggleValue;
      lastBlink = millis();
    } 
    if (toggleValue == true) {
      Display::Print(DispData_s.menuX_u8 + DispData_s.menuVal_u8, DispData_s.selectedMenuIdx_u8 + 2, buff, 0, 1); /* highlight text */
    } else {
      Display::Print(DispData_s.menuX_u8 + DispData_s.menuVal_u8, DispData_s.selectedMenuIdx_u8 + 2, buff);  
    }
  } else if (toggleValue == true) { /* if edit mode released but text highlight still active deactivate here */
    //char buff[5];
    //snprintf(buff, 5, "%4d", Disp.CurrentValue_p->value);
    //Display::Print(Disp.menuX + Disp.menuVal, Disp.menuY + Disp.selectedMenuIdx + 2, buff);  
    toggleValue = false;
  }
}

void runDisplay(Key_EnumType* key, TimeType* time) {
  DispData_s.Key_p = key;
  DispData_s.Time_p = time;
  switch(*DispData_s.Key_p){
    /*case KEY_RIGHT:*/
    case KEY_CENTER:  keyCenterEvent(); break;
    case KEY_DOWN:    keyDownEvent(); break;
    /*case KEY_LEFT:    keyLeftEvent(); break;*/
    case KEY_UP:      keyUpEvent(); break;
    case KEY_NONE:
    default:          keyReleasedEvent(); break;
    
  }

  setCursor();
  showCursor();
  menuUpdate();
  editValue();
}
