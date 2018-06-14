#include "display.h"
#include "types.h"
#include "Time.h"
#include "utils.h"
#include "control.h"

//#define FIRST_RUN
//#define DEBUG_ENABLED
#define DISPLAY_SCREENSAVER 10 /* wait 10 seconds */
static Key_EnumType key;
uint32_t lastTimeKeyPress;
static TimeType time;
static boolean screensaveEnable;

void setup() {
  #if defined(DEBUG_ENABLED)
    Serial.begin(SERIAL_BAUD_115200);
  #endif
  initDisplay(); /* set up the main menu */  
  initTime(time); /* build up connection to rtc */
  parseCompileTime(&time);
  printTime(&time);
  #if defined(FIRST_RUN)
    setTime(&time);
  #endif
  init_time(&time);/* set default values, names, .. */
  init_setting();  /* set default values, names, .. */
  #if defined(FIRST_RUN)
    NVM::writeAllValues(); /* save all values which are set in init_setting() */
  #else
   NVM::loadAllValues(); /* read data from eeprom */
  #endif  
  init_control(&time);
}

void loop() {
  readTime(&time);
  /* display loop */
  u8g.firstPage();  
  do {
    readKey(key); /* get key in the display loop otherwise it ll stuck */
    if (key != KEY_NONE) {
      /* save time when last user input came */
      lastTimeKeyPress = millis()/1000;
      if (screensaveEnable == true) {
        screensaveEnable = false;  
        DispData_s.selectedMenuIdx_u8 = Menu::MENU_MAIN_OUTPUT;
        DispData_s.selectedMenu = Menu::MAIN_MENU;
        key = KEY_NONE;
      }
    }
    /* check if it is time to display the screen saver */
    if (millis()/1000 - lastTimeKeyPress > DISPLAY_SCREENSAVER) {
      screensaveEnable = true;
    }

    if (screensaveEnable == true) {
      showScreenSaver(time);
    } else {
      runDisplay(&key, &time);
    }
  } while(u8g.nextPage());
  
  run_control(&WB);
}





