#include "display.h"
#include "types.h"
#include "Time.h"
#include "utils.h"
#include "control.h"

//#define FIRST_RUN
//#define DEBUG_ENABLED
#define DISPLAY_SCREENSAVER 10 /* wait 10 seconds */

static TimeType time;


void setup() {
  #if defined(DEBUG_ENABLED)
    Serial.begin(SERIAL_BAUD_115200);
  #endif
  Display_Init(&time); /* set up the main menu and pass a pointer to the time to it */  
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
  static uint32_t lastTimeKeyPress; 
  static boolean screensaveEnable_bo;

  readTime(&time);
  /* display loop */
  u8g.firstPage();  
    do {
        if (HmiData_s.KeyPad_s.Key_e != KEY_NONE) {
            /* save time when last user input came */
            lastTimeKeyPress = millis()/1000;
            if (screensaveEnable_bo == true) {
            screensaveEnable_bo = false;  
            Display_Init(&time);
            //HmiData_s.selectedMenuIdx_u8 = Menu::MENU_MAIN_OUTPUT;
            //HmiData_s.selectedMenu_e = Menu::MAIN_MENU;
            //HmiData_s.KeyPad_s.Key_e = KEY_NONE;
            }
        }
        /* check if it is time to display the screen saver */
        if (millis()/1000 - lastTimeKeyPress > DISPLAY_SCREENSAVER) {
            screensaveEnable_bo = true;
        }
        Display_Main(screensaveEnable_bo);
    } while(u8g.nextPage());
  
  run_control(&WB);
}





