#include "U8glib/src/U8glib.h"

#define J1      'X'
#define J2      'O'
#define JNONE   '.'
#define CASE_SELECTED '_'

#define CASE_W  25
#define CASE_H  13
#define LINE_H  1
#define LINE_W  1
#define FONT_H  9


//////////////
// JOYSTICK //
//////////////

#define JSK_NONE    -1
#define JSK_LEFT    0
#define JSK_CLICK   1
#define JSK_BOTTOM  2
#define JSK_RIGHT   3
#define JSK_TOP     4


class Case {
private:
    int x;
    int y;
    char joueur;

public:
    Case();
    Case(int x, int y) {
        this->x = x;
        this->y = y;
        this->joueur = JNONE;
    }
    Case(int x, int y, char joueur) {
        this->x = x;
        this->y = y;
        this->joueur = joueur;
    }
    ~Case();

    
    ////////////
    // GETTER //
    ////////////

    int getX(){
        return this->x;
    }
    int getY(){
        return this->y;
    }
    char getJoueur() {
        return this->joueur;
    }


    ////////////
    // SETTER //
    ////////////
    
    void setX(int x) {
        return this->x = x;
    }
    void setY(int y) {
        return this->y = y;
    }
    void setJoueur(char j) {
        this->joueur = j;
    }


    char *print() {
        return &(this->joueur);
    }

    bool equals(Case *other) {
        return (this->getY() == other->getY()) && (this->getX() == other->getX());
    }
};


class Grille {
private:
    Case *cases[3][3];
    Case *selected;
    char joueur;
    bool redraw;
    uint8_t keyCodePrev;
    
    void drawTitleBox(U8GLIB_NHD_C12864 *u8g, char *message);
    void buildGrid(U8GLIB_NHD_C12864 *u8g, int xInit, int yInit);
    void drawCases(U8GLIB_NHD_C12864 *u8g, int xInit, int yInit);

    bool canPlay(int x, int y) {
        return this->cases[x-1][y-1]->getJoueur() == JNONE;
    }

    bool checkD11(){
        bool haveWinner = true;

        if (this->selected->getX() == 1 && this->selected->getY() ==1) {
            for (int i = 0; i < 3; i++) {
                if (cases[i][i]->getJoueur() != this->selected->getJoueur()) {
                    haveWinner = false;
                    break;
                }
            }
        }

        if (haveWinner) {
            return this->selected->getJoueur();
        }
    }

    bool checkD13(){
        bool haveWinner = true;

        if (this->selected->getX() == 1 && this->selected->getY() ==3) {
            for (int i = 0; i < 3; i++) {
                if (cases[0+i][2-i]->getJoueur() != this->selected->getJoueur()) {
                    haveWinner = false;
                    break;
                }
            }
        }
        return haveWinner;
    }

    bool checkD31(){
        bool haveWinner = true;

        if (this->selected->getX() == 3 && this->selected->getY() == 1) {
            for (int i = 0; i < 3; i++) {
                if (cases[2-i][0+i]->getJoueur() != this->selected->getJoueur()) {
                    haveWinner = false;
                    break;
                }
            }
        }
        return haveWinner;
    }

    bool checkD33(){
        bool haveWinner = true;

        if (this->selected->getX() == 3 && this->selected->getY() == 3) {
            for (int i = 0; i < 3; i++) {
                if (this->cases[2-i][2-i]->getJoueur() != this->selected->getJoueur()) {
                    haveWinner = false;
                    break;
                }
            }
        }
        return haveWinner;
    }

    /**
    * Methode qui retourne le nom du gagne (dernier joueur, ou JNONE).
    **/
    char *checkWinner() {
        char *lastJoueur = this->selected->getJoueur();

        // check selected line
        bool haveWinner = true;
        for (int i = 0; i < 3; i++) {
            if (this->cases[this->selected->getX()-1][i]->getJoueur() != lastJoueur) {
                haveWinner = false;
                break;
            }
        }

        if (haveWinner) {
            return lastJoueur;
        } else {
            // check selected column
            for (int i = 0; i < 3; i++) {
                if (this->cases[i][this->selected->getY()-1]->getJoueur() != lastJoueur) {
                    haveWinner = false;
                    break;
                }
            }
        }

        if (haveWinner) {
            return lastJoueur;
        }

        // check diagonal where X = 1 (adapt with Y value)
        if (selected->getX()==1) {
            if (selected->getY()==1) {
                haveWinner = checkD11();
            } else if (selected->getY() ==3){
                haveWinner = checkD13();
            }
        }

        if (haveWinner) {
            return lastJoueur;
        }

        // check diagonal where X = 3 (adapt with Y value)
        if (selected->getX()==3) {
            if (selected->getY()==1) {
                haveWinner = checkD31();
            } else if (selected->getY() ==3){
                haveWinner = checkD33();
            }
        }

        if (selected->getX()==2 && selected->getY()==2){
            if (checkD11() || checkD13() || checkD31() || checkD33()) {
                return lastJoueur;
            }
        }

        return JNONE;
    }

public:
    Grille() {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                this->cases[i][j] = new Case(i+1, j+1);
            }
        }
        this->selected = this->cases[0][0];
        this->joueur = J1;
        this->redraw = true;
        this->keyCodePrev = JSK_NONE;
    }
    ~Grille() {
        delete this->cases;
    }

    char getJoueurCourant() { return this->joueur; }
    Case *getCaseSelected() { return this->selected; }
    bool needsRedraw()      { return this->redraw; }
    void drawed()           { this->redraw = false; }
    
    void moveCursor(uint8_t keyCode);

    void draw(U8GLIB_NHD_C12864 *u8g) {
        this->drawTitleBox(u8g, this->joueur);
        this->buildGrid(u8g, CASE_W, CASE_H+1);
        this->drawCases(u8g, CASE_W, CASE_H+1);
    }

    void playTurn(char joueur, int x, int y) {
        this->cases[x-1][y-1]->setJoueur(joueur);
        if (this->joueur == J1) {
            this->joueur = J2;
        } else {
            this->joueur = J1;
        }
        this->checkWinner();
    }
};





void Grille::buildGrid(U8GLIB_NHD_C12864 *u8g, int xInit, int yInit) {
    // Setup graphical configuration
    int yAbs = yInit;
    u8g_uint_t width = u8g->getWidth();
    u8g_uint_t height = u8g->getHeight();
    u8g->setDefaultForegroundColor();

    // Draw columns
    int yIncr = 3*(CASE_H+1);
    for (int j = 0; j <= 3; j++) {
      int x = xInit + j * (CASE_W + LINE_W);
      u8g->drawLine(x, yAbs, x, yAbs+yIncr);
    }

    // Draw lines
    for (int line = 1; line <= 3; line++) {
      u8g->drawLine(CASE_W, yAbs, width-CASE_W, yAbs);
      yAbs += CASE_H + LINE_H;
    }
    u8g->drawLine(CASE_W, yAbs, width-CASE_W, yAbs);
    yAbs += CASE_H/2 + LINE_H;
}


void Grille::drawTitleBox(U8GLIB_NHD_C12864 *u8g, char *message) {
    u8g->setDefaultForegroundColor();
    u8g->drawBox(0, 0, u8g->getWidth(), FONT_H);
    u8g->setDefaultBackgroundColor();
    u8g->drawStr((u8g->getWidth() - u8g->getStrWidth(message))/2, 0, message);
}

void Grille::drawCases(U8GLIB_NHD_C12864 *u8g, int xInit, int yInit) {  
    int yCoord;
    int xCoord;
    for (int i = 0; i < 3; i++) {
        yCoord = yInit + i*(CASE_H + LINE_H) + CASE_H/3;
        xCoord = xInit + CASE_W/2 - 1;

        for (int j = 0; j < 3; j++) {
            if (this->cases[i][j]->equals(this->selected) && this->cases[i][j]->getJoueur()==JNONE) {
                // u8g->drawStr(xCoord, yCoord, "_");
            } else {
                u8g->drawStr(xCoord, yCoord, this->cases[i][j]->print());
            }

            xCoord += (CASE_W + LINE_W);
        }
    }
}

void Grille::moveCursor(uint8_t keyCode) {
    if (keyCode == this->keyCodePrev) {
        return;
    }

    int x = this->selected->getX();
    int y = this->selected->getY();

    this->redraw = true;
    
    switch (keyCode) {
        case JSK_LEFT:
            if (x == 1) this->selected->setX(3);
            else        this->selected->setX(x-1);
            break;
        case JSK_RIGHT:
            if (x == 3) this->selected->setX(1);
            else        this->selected->setX(x+1);
            break;
        case JSK_TOP:
            if (y == 1) this->selected->setY(3);
            else        this->selected->setY(y-1);
            break;
        case JSK_BOTTOM:
            if (y == 3) this->selected->setY(1);
            else        this->selected->setY(y+1);
            break;
        case JSK_CLICK:
            if(this->canPlay(x,y)) {
                this->playTurn(this->joueur, x, y);
            }
            break;
        default:
            this->redraw = false;
            break;
    }

    this->keyCodePrev = keyCode;
}