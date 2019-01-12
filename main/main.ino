#include "display.h"
#include "types.h"
#include "Time.h"
#include "utils.h"
#include "control.h"

/*************************************************/
/*         LOCAL VARIABLE DECLARATION            */
/*************************************************/
static WateringBoy_DataType WB;
static TimeType time;
static DISPLAY_MODE DisplayMode_e;

void setup() {
    DEBUG_INIT(SERIAL_BAUD_115200);
    Display_Init(&WB, &time);    /* set up the main menu and pass a pointer to the time to it */
    Time_Init(time);        /* build up connection to rtc */
    NVM::Setting_Init(&WB, &time);    /* set default values, names, .. */
    
    if (IS_ENABLED(FIRST_RUN))
        NVM::WriteAllValues(&WB); /* save all default values to eeprom */
    else 
        NVM::GetAllValues(&WB); /* read data from eeprom and overwrite default values */

    Control_Init(&time);
    DisplayMode_e = DISPLAY_NORMAL;
}

void loop() {
    static uint32_t lastTimeKeyPress_u32;
    static int8_t currentOutlet_s8 = INVALID_VALVE_IDX;

    GetTime(&time);
    /* display loop */
    u8g.firstPage();  
    do {
        if (0 <= currentOutlet_s8) { 
            DisplayMode_e = DISPLAY_WATERING;
        } else { 
            if (HmiData_s.KeyPad_s.Key_e != KEY_NONE) {
                /* save time when last user input came */
                lastTimeKeyPress_u32 = millis()/1000;
                if (   (DisplayMode_e == DISPLAY_SCREENSAVER)
                    || (DisplayMode_e == DISPLAY_INIT)) {
                    DisplayMode_e = DISPLAY_NORMAL;
                    Display_Init(&WB, &time); /* start with start screen */
                }
                else if (DisplayMode_e == DISPLAY_STANDBY) {
                    DisplayMode_e = DISPLAY_INIT;
                }
            } else {
                /* no key are pressed. Check if it is time to display the screen saver */
                int16_t deltaTime_s16 = (int16_t)(millis() / 1000 - lastTimeKeyPress_u32);
                if (   (deltaTime_s16 > WB.Display_s.ScreenSaver_s.Value_s16)
                    && (INVALID_VALVE_IDX != currentOutlet_s8)) {
                    if (deltaTime_s16 > WB.Display_s.Sleep_s.Value_s16) {
                        DisplayMode_e = DISPLAY_STANDBY;
                    } else {
                        DisplayMode_e = DISPLAY_SCREENSAVER;
                    }
                }
            }
        }

        Display_Main(DisplayMode_e);
    } while(u8g.nextPage());
  
    currentOutlet_s8 = Control_Main(&WB);
}





