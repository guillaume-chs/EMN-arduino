
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

#define NONE    -1
#define LEFT    0
#define CLICK   1
#define BOTTOM  2
#define RIGHT   3
#define TOP     4

uint8_t uiKeyCodeFirst = NONE;
uint8_t uiKeyCodeSecond = NONE;
uint8_t uiKeyCode = NONE;

int key = NONE;
int oldkey = NONE;


//////////////
// JOYSTICK //
//////////////

#define STEP_MENU 1
#define STEP_GAME 2

uint8_t STEP = STEP_MENU;



////////////
//  MENU  //
////////////

#define MENU_ITEMS 4
char *menu_strings[MENU_ITEMS] = { "Damien", "Steeve", "Guillaume", "C'est parti !" };

uint8_t menu_current = 0;
uint8_t last_key_code = NONE;
bool menu_redraw_required = true;
bool game_redraw_required = false;




/////////////
//  Setup  //
/////////////

void uiStep(void);
void drawMenu(void);
void drawGame(void);
void updateMenu(void);
int get_joystick_key(unsigned int);
char *get_joystick_name(int key);

U8GLIB_NHD_C12864 u8g(13, 11, 10, 9, 8);

void setup() {
  u8g.setRot180();      // rotate screen, if required
  u8g.setContrast(0);

  u8g.setFont(u8g_font_6x12);//4x6 5x7 5x8 6x10 6x12 6x13
  u8g.setFontRefHeightText();
  u8g.setFontPosTop();
  
  menu_redraw_required = true; // force initial redraw
  game_redraw_required = false;
}

TASK(periodicTask) {  
  uiStep();              // check for key press
  
  if (STEP == STEP_MENU) {
    updateMenu();          // update menu bar
    
    if (menu_redraw_required) {
      u8g.firstPage();
      do {
        drawMenu();
      } while(u8g.nextPage());
      menu_redraw_required = false;
    }
  } else {
    if (game_redraw_required) {
      u8g.firstPage();
      do {
        drawGame();
      } while(u8g.nextPage());
      game_redraw_required = false;
    }
  }
}





void uiStep(void) {
  key = get_joystick_key(analogRead(0));

  if (key != oldkey) {			
    oldkey = key;
    if (key > NONE) {
      uiKeyCodeFirst = key;
      uiKeyCode = uiKeyCodeFirst;           
    }
  }
}





void drawMenu(void) {
  uint8_t i, h;
  u8g_uint_t w, d;
  
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

void drawGame(void) {
  uint8_t i, font_height;
  u8g_uint_t width, height, width_ratio;

  font_height = u8g.getFontAscent() - u8g.getFontDescent();
  width = u8g.getWidth();
  height = u8g.getHeight();
  width_ratio = (width/5);

  u8g.setDefaultForegroundColor();
  
  // First line
  i = 0;
  u8g.drawBox(0, i, width, font_height);
  u8g.setDefaultBackgroundColor();
  u8g.drawStr(3, i, "J1 : 0");
  u8g.drawStr(width - 3 - u8g.getStrWidth("3.4 : Damien"), i, "3.4 : Damien");
  i = font_height*1.5 + 1;
  u8g.setDefaultForegroundColor();

  // Column lines
  int y = 3*(font_height+1);
  for (int j = 0; j <= 3; j++) {
    int x = (1+j)*(width_ratio+1);
    u8g.drawLine(x, i, x, i+y);
  }

  // Grid
  for (int line = 1; line <= 3; line++) {
    u8g.drawLine(width_ratio, i, width-width_ratio, i);
    i += font_height + 1;
  }
  u8g.drawLine(width_ratio, i, width-width_ratio, i);
  i += font_height/2 + 1;

  u8g.drawBox(0, i, width, font_height);
}

void updateMenu(void) {
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

    case CLICK:
      if (menu_current == MENU_ITEMS - 1) {
        STEP = STEP_GAME;
        menu_redraw_required = false;
        game_redraw_required = true;
      }
  }
  uiKeyCode = NONE;
}






// Convert ADC value to key number
//         4
//         |
//   0 --  1 -- 3
//         |
//         2
int get_joystick_key(unsigned int input) {   
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
    case LEFT:   name = "LEFT";   break;
    case RIGHT:  name = "RIGHT";  break;
    case TOP:    name = "TOP";    break;
    case BOTTOM: name = "BOTTOM"; break;
    case CLICK:  name = "CLICK";  break;
    default:     name = "NONE";   break;
  }
  return name;
}