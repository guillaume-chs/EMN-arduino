
#include "tpl_os.h"
#include "Arduino.h"
#include "lcd-gds.h"


/////////////////
// SHIELD PINS //
/////////////////

#define SCK   13
#define MOSI  11
#define CS    10
#define A0    9
#define RST   8
#define JOYSTICK 	A0


//////////////
// JOYSTICK //
//////////////

int key = JSK_NONE;
int oldkey = JSK_NONE;
uint8_t uiKeyCode = JSK_NONE;


////////////
//  MENU  //
////////////

#define MENU_ITEMS 4
char *menu_strings[MENU_ITEMS] = { "Damien", "Steeve", "Guillaume", "C'est parti !" };

uint8_t menu_current = 0;
uint8_t last_key_code = JSK_NONE;
bool menu_redraw_required = true;


///////////
// STEPS //
///////////

#define STEP_MENU 1
#define STEP_GAME 2

uint8_t STEP = STEP_MENU;





/////////////
//  Setup  //
/////////////

void uiStep(void);
void drawMenu(void);
void drawGame(void);
void updateMenu(void);
void updateGame(void);
int get_joystick_key(unsigned int);
char *get_joystick_name(int key);

U8GLIB_NHD_C12864 u8g(SCK, MOSI, CS, A0, RST);
Grille grid;

void setup() {
  u8g.setRot180();      // rotate screen, if required
  u8g.setContrast(0);

  u8g.setFont(u8g_font_6x12);//4x6 5x7 5x8 6x10 6x12 6x13
  u8g.setFontRefHeightText();
  u8g.setFontPosTop();

  Serial.begin(9600);
  
  menu_redraw_required = true; // force initial redraw
}

TASK(periodicTask) {  
  uiStep();              // check for key press
  
  if (STEP == STEP_MENU) {
    // update menu bar
    updateMenu();

    // draw menu
    if (menu_redraw_required) {
      u8g.firstPage();
      do {
        drawMenu();
      } while(u8g.nextPage());
      menu_redraw_required = false;
    }
  }

  else if (STEP == STEP_GAME) {
    // update game
    grid.moveCursor(uiKeyCode);
    uiKeyCode = JSK_NONE;

    if (grid.needsRedraw()) {
      u8g.firstPage();
      do {
        grid.draw(&u8g);
      } while(u8g.nextPage());
      grid.drawed();
    }
  }
}





//////////////
// CONTROLS //
//////////////

void uiStep(void) {
  key = get_joystick_key(analogRead(0));

  if (key != oldkey) {			
    oldkey = key;
    if (key > JSK_NONE) {
      uiKeyCode = key;         
    }
  }
}





////////////////////
// MENU RENDERING //
////////////////////

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

void updateMenu(void) {
  switch (uiKeyCode) {
    case JSK_BOTTOM:
      menu_current++;
      if (menu_current >= MENU_ITEMS) {
        menu_current = 0;
      }
      menu_redraw_required = true;
      break;

    case JSK_TOP:
      if (menu_current == 0) {
        menu_current = MENU_ITEMS;
      }
      menu_current--;
      menu_redraw_required = true;
      break;

    case JSK_CLICK:
      if (menu_current == MENU_ITEMS - 1) {
        STEP = STEP_GAME;
        menu_redraw_required = false;
      }
      break;
  }
  uiKeyCode = JSK_NONE;
}





// Convert ADC value to key number
//         4
//         |
//   0 --  1 -- 3
//         |
//         2
int get_joystick_key(unsigned int input) {   
    if (input < 100) return JSK_LEFT;
    else  if (input < 300) return JSK_CLICK;
    else  if (input < 500) return JSK_BOTTOM;
    else  if (input < 700) return JSK_RIGHT;
    else  if (input < 900) return JSK_TOP;    
    else  return JSK_NONE;
}

char *get_joystick_name(int key) {
  char *name;
  
  switch(key) {
    case JSK_LEFT:   name = "LEFT";   break;
    case JSK_RIGHT:  name = "RIGHT";  break;
    case JSK_TOP:    name = "TOP";    break;
    case JSK_BOTTOM: name = "BOTTOM"; break;
    case JSK_CLICK:  name = "CLICK";  break;
    default:         name = "NONE";   break;
  }
  return name;
}