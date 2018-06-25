#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);

namespace Display {
    enum DISPLAY_LINE_TAG {
        LINE_1 = 8,
        LINE_HORIZONTAL = 10,
        LINE_2 = 20,
        LINE_3 = 28,
        LINE_4 = 36,
        LINE_5 = 44,
        LINE_6 = 52,
        LINE_7 = 60,
    };
    
  void Init() {
    if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
      u8g.setColorIndex(255);     // white
    }
    else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
      u8g.setColorIndex(3);         // max intensity
    }
    else if ( u8g.getMode() == U8G_MODE_BW ) {
      u8g.setColorIndex(1);        
    }
  }

  void Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2){
    u8g.drawLine(x1, y1, x2, y2);
  }

  void Box(uint8_t x, uint8_t y, uint8_t w, uint8_t h){
    u8g.drawBox(x, y, w, h);
  }

  void HLine(uint8_t y_u8) {
      u8g.drawLine(1, y_u8, 128, y_u8);
  }

  void Print(uint8_t x, uint8_t y, const char text[], uint8_t fontSize = 0, uint8_t fontType = 0) {
    if (fontSize == 0) {
      if (fontType == 0) {
        //u8g.setFont(u8g_font_unifont); u8g.setFont(u8g_font_u8glib_4);//u8g_font_m2icon_5);//u8g_font_6x12_75r); 
        u8g.setFont(u8g_font_6x10);
        u8g.drawStr(x, y, text);
      } else {
        uint8_t w = 28;//Disp.menuUnit - Disp.menuVal;
        u8g.drawBox(x-2, y - 8, w, 8);
        u8g.setColorIndex(0);
        u8g.drawStr(x, y, text);
        u8g.setColorIndex(1);
      }
    } else {
      /* display big font */
      u8g.setFont(u8g_font_9x18);
      u8g.drawStr(x, y*20, text);
    }
  }

  void PrintXY(uint8_t pX, uint8_t pY, char text[]) {
    u8g.setFont(u8g_font_6x10);
    u8g.drawStr(pX, pY, text);
  }

  void Sleep() {
      u8g.sleepOn();
  }
  void Awake() {
      u8g.sleepOff();
  }
}
