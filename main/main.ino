#include "display.h"
#include "types.h"
#include "Time.h"
#include "utils.h"
#include "control.h"

static TimeType time;
DISPLAY_MODE DisplayMode_e;

void setup() {
    DEBUG_INIT(SERIAL_BAUD_115200);
    Display_Init(&time); /* set up the main menu and pass a pointer to the time to it */  
    Time_Init(time); /* build up connection to rtc */
    
    SettingTime_Init(&time);/* set default values, names, .. */
    Setting_Init();  /* set default values, names, .. */
    #if defined(FIRST_RUN)
        NVM::SetSettings(); /* save all values which are set in init_setting() */
    #else
        NVM::GetSettings(); /* read data from eeprom */
    #endif  
    Control_Init(&time);
    DisplayMode_e = DISPLAY_NORMAL;
}

void loop() {
    static uint32_t lastTimeKeyPress_u32; 
    GetTime(&time);
    /* display loop */
    u8g.firstPage();  
    do {
        if (HmiData_s.KeyPad_s.Key_e != KEY_NONE) {
            /* save time when last user input came */
            lastTimeKeyPress_u32 = millis()/1000;
            if (   (DisplayMode_e == DISPLAY_SCREENSAVER)
                || (DisplayMode_e == DISPLAY_INIT)) {
                DisplayMode_e = DISPLAY_NORMAL;
                Display_Init(&time); /* start with start screen */
            }
            else if (DisplayMode_e == DISPLAY_STANDBY) {
                DisplayMode_e = DISPLAY_INIT;
            }
        } else {
            /* no key are pressed. Check if it is time to display the screen saver */
            int16_t deltaTime_s16 = (int16_t)(millis() / 1000 - lastTimeKeyPress_u32);
            if (deltaTime_s16 > WB.Display.ScreenSaver.value) {
                if (deltaTime_s16 > WB.Display.Sleep.value) {
                    DisplayMode_e = DISPLAY_STANDBY;
                } else {
                    DisplayMode_e = DISPLAY_SCREENSAVER;
                }
            }
        }
        Display_Main(DisplayMode_e);
    } while(u8g.nextPage());
  
  Control_Main(&WB);
}





