
#include "tpl_os.h"
#include "Arduino.h"
#include "U8glib/src/U8glib.h"


/////////////////
// SHIELD PINS //
/////////////////

// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9, RST = 8
#define SCK   13
#define MOSI  11
#define CS    10
#define A0    9
#define RST   8
#define JOYSTICK 	A0


//////////////
// JOYSTICK //
//////////////

#define NONE  -1
#define LEFT  0
#define CLICK   1
#define BOTTOM  2
#define RIGHT   3
#define TOP   4

uint8_t uiKeyCodeFirst = NONE;
uint8_t uiKeyCodeSecond = NONE;
uint8_t uiKeyCode = NONE;

int key = NONE;
int oldkey = NONE;


////////////
//  MENU  //
////////////

#define MENU_ITEMS 4
char *menu_strings[MENU_ITEMS] = { "Ping", "Pong", "Pouet", "Click" };

uint8_t menu_current = 0;
uint8_t last_key_code = NONE;
bool menu_redraw_required = true;




/////////////
//  Setup  //
/////////////

void uiStep(void);
void drawMenu(void);
void updateMenu(void);
int get_joystick_key(unsigned int);
char *get_joystick_name(int key);

U8GLIB_NHD_C12864 u8g(13, 11, 10, 9, 8);

void setup() {
  Serial.begin(9600);
  u8g.setRot180();      // rotate screen, if required
  u8g.setContrast(0);
  menu_redraw_required = true; // force initial redraw
}

TASK(periodicTask) {  
  uiStep();              // check for key press
  updateMenu();          // update menu bar    
  
  if (menu_redraw_required) {
    u8g.firstPage();
    do  {
      drawMenu();
    } while(u8g.nextPage());
    menu_redraw_required = false;
  }
}





void uiStep(void) {
  key = get_joystick_key(analogRead(0));

  if (key != oldkey) {			
    oldkey = key;
    if (key > NONE) {
      Serial.println(get_joystick_name(key));
      uiKeyCodeFirst = key;
      uiKeyCode = uiKeyCodeFirst;           
    }
  }
}





void drawMenu(void) {
  uint8_t i, h;
  u8g_uint_t w, d;

  u8g.setContrast(0);
  u8g.setFont(u8g_font_6x12);//4x6 5x7 5x8 6x10 6x12 6x13
  u8g.setFontRefHeightText();
  u8g.setFontPosTop();
  
  h = u8g.getFontAscent() - u8g.getFontDescent();
  w = u8g.getWidth();
  for (i = 0; i < MENU_ITEMS; i++) {
    d = (w-u8g.getStrWidth(menu_strings[i]))/2;
    u8g.setDefaultForegroundColor();
    if (i == menu_current) {  
      u8g.drawBox(0, i*h+1, w, h);
      u8g.setDefaultBackgroundColor();
    }
    u8g.drawStr(d, i*h+1, menu_strings[i]);
  }
}

void updateMenu(void) 
{
  switch (uiKeyCode) {
    case BOTTOM:
      menu_current++;
      if (menu_current >= MENU_ITEMS) {
        menu_current = 0;
      }
      menu_redraw_required = true;
      break;
    case TOP:
      if (menu_current == 0) {
        menu_current = MENU_ITEMS;
      }
      menu_current--;
      menu_redraw_required = true;
      break;
  }
  uiKeyCode = CLICK;
}






// Convert ADC value to key number
//         4
//         |
//   0 --  1 -- 3
//         |
//         2
int get_joystick_key(unsigned int input)
{   
    if (input < 100) return LEFT;
    else  if (input < 300) return CLICK;
    else  if (input < 500) return BOTTOM;
    else  if (input < 700) return RIGHT;
    else  if (input < 900) return TOP;    
    else  return NONE;
}

char *get_joystick_name(int key) {
  char *name;

  switch(key) {
    case LEFT:
      name = "LEFT";
      break;

    case RIGHT:
      name = "RIGHT";
      break;

    case TOP:
      name = "TOP";
      break;

    case BOTTOM:
      name = "BOTTOM";
      break;

    case CLICK:
      name = "CLICK";
      break;

    default:
      name = "NONE";
      break;
  }

  return name;
}
