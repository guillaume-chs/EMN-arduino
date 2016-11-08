#include "U8glib/src/U8glib.h"

#define J1      'X'
#define J2      'O'
#define JNONE   '.'
#define CASE_SELECTED "_"

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
    int line;
    int col;
    char joueur;

public:
    Case();
    Case(int line, int col) {
        this->line = line;
        this->col = col;
        this->joueur = JNONE;
    }
    Case(int line, int col, char joueur) {
        this->line = line;
        this->col = col;
        this->joueur = joueur;
    }
    ~Case();

    
    ////////////
    // GETTER //
    ////////////

    int getLine(){
        return this->line;
    }
    int getCol(){
        return this->col;
    }
    char getJoueur() {
        return this->joueur;
    }


    ////////////
    // SETTER //
    ////////////
    
    void setLine(int line) {
        return this->line = line;
    }
    void setCol(int col) {
        return this->col = col;
    }
    void setJoueur(char j) {
        this->joueur = j;
    }


    char *print() {
        return &(this->joueur);
    }

    bool equals(Case *other) {
        return (this->getLine() == other->getLine()) && (this->getCol() == other->getCol());
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

    bool canPlay(int line, int col) {
        return this->cases[line-1][col-1]->getJoueur() == JNONE;
    }

    bool checkD11(){
        bool haveWinner = true;

        if (this->selected->getLine() == 1 && this->selected->getCol() ==1) {
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

        if (this->selected->getLine() == 1 && this->selected->getCol() == 3) {
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

        if (this->selected->getLine() == 3 && this->selected->getCol() == 1) {
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

        if (this->selected->getLine() == 3 && this->selected->getCol() == 3) {
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
        for (int col = 0; col < 3; col++) {
            if (this->cases[this->selected->getLine()-1][col]->getJoueur() != lastJoueur) {
                haveWinner = false;
                break;
            }
        }

        if (haveWinner) {
            return lastJoueur;
        } else {
            // check selected column
            for (int line = 0; line < 3; line++) {
                if (this->cases[line][this->selected->getCol()-1]->getJoueur() != lastJoueur) {
                    haveWinner = false;
                    break;
                }
            }
        }

        if (haveWinner) {
            return lastJoueur;
        }

        // check diagonal where X = 1 (adapt with Y value)
        if (selected->getLine()==1) {
            if (selected->getCol()==1) {
                haveWinner = checkD11();
            } else if (selected->getCol()==3){
                haveWinner = checkD13();
            }
        }

        if (haveWinner) {
            return lastJoueur;
        }

        // check diagonal where X = 3 (adapt with Y value)
        if (selected->getLine()==3) {
            if (selected->getCol()==1) {
                haveWinner = checkD31();
            } else if (selected->getCol() ==3){
                haveWinner = checkD33();
            }
        }

        if (selected->getLine()==2 && selected->getCol()==2){
            if (checkD11() || checkD13() || checkD31() || checkD33()) {
                return lastJoueur;
            }
        }

        return JNONE;
    }

public:
    Grille() {
        for (int line = 0; line < 3; line++) {
            for (int col = 0; col < 3; col++) {
                this->cases[line][col] = new Case(line+1, col+1);
            }
        }
        // DINGUE, on doit faire ça
        this->cases[2][2] = new Case(3, 3);

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
    void draw(U8GLIB_NHD_C12864 *u8g);
    void playTurn(char joueur, int line, int col);
};





void Grille::draw(U8GLIB_NHD_C12864 *u8g) {
    if (this->joueur == J1) {
        this->drawTitleBox(u8g, "Au J1 de jouer");
    } else if (this->joueur == J2) {
        this->drawTitleBox(u8g, "Au J2 de jouer");
    }
    this->buildGrid(u8g, CASE_W, CASE_H+1);
    this->drawCases(u8g, CASE_W, CASE_H+1);
}

void Grille::buildGrid(U8GLIB_NHD_C12864 *u8g, int xInit, int yInit) {
    // Setup graphical configuration
    int yAbs = yInit;
    u8g_uint_t width = u8g->getWidth();
    u8g_uint_t height = u8g->getHeight();
    u8g->setDefaultForegroundColor();

    // Draw columns
    int yIncr = 3*(CASE_H+1);
    for (int col = 0; col < 4; col++) {
      int x = xInit + col * (CASE_W + LINE_W);
      u8g->drawLine(x, yAbs, x, yAbs+yIncr);
    }

    // Draw lines
    for (int line = 0; line < 3; line++) {
      u8g->drawLine(CASE_W, yAbs, width-CASE_W, yAbs);
      yAbs += CASE_H + LINE_H;
    }
    u8g->drawLine(CASE_W, yAbs, width-CASE_W, yAbs);
    yAbs += CASE_H/2 + LINE_H;

    u8g->drawBox(0, yAbs, width, yAbs);
}

void Grille::drawTitleBox(U8GLIB_NHD_C12864 *u8g, char *message) {
    u8g->setDefaultForegroundColor();
    u8g->drawBox(0, 0, u8g->getWidth(), FONT_H);
    u8g->setDefaultBackgroundColor();
    u8g->drawStr((u8g->getWidth() - u8g->getStrWidth(message))/2, 0, message);
}

void Grille::drawCases(U8GLIB_NHD_C12864 *u8g, int xInit, int yInit) {
    u8g->setDefaultForegroundColor();
    
    int yCoord;
    int xCoord;
    for (int line = 0; line < 3; line++) {
        yCoord = yInit + line*(CASE_H + LINE_H) + CASE_H/3;
        xCoord = xInit + CASE_W/2 - 1;

        for (int col = 0; col < 3; col++) {
            if (this->cases[line][col]->equals(this->selected) && this->cases[line][col]->getJoueur()==JNONE) {
                u8g->drawStr(xCoord, yCoord, CASE_SELECTED);
            } else {
                u8g->drawStr(xCoord, yCoord, this->cases[line][col]->print());
            }

            xCoord += (CASE_W + LINE_W);
        }
    }
}

void Grille::moveCursor(uint8_t keyCode) {
    if (keyCode == this->keyCodePrev) {
        return;
    }

    int lineI = this->selected->getLine() - 1;
    int colI = this->selected->getCol() - 1;

    this->redraw = true;
    
    switch (keyCode) {
        case JSK_LEFT:
            if (colI == 0) this->selected = this->cases[lineI][2];
            else           this->selected = this->cases[lineI][colI-1];
            break;
        case JSK_RIGHT:
            if (colI == 2) this->selected = this->cases[lineI][0];
            else           this->selected = this->cases[lineI][colI+1];
            break;
        case JSK_TOP:
            if (lineI == 0) this->selected = this->cases[2][colI];
            else            this->selected = this->cases[lineI-1][colI];
            break;
        case JSK_BOTTOM:
            if (lineI == 2) this->selected = this->cases[0][colI];
            else            this->selected = this->cases[lineI+1][colI];
            break;
        case JSK_CLICK:
            if(this->canPlay(lineI+1,colI+1)) {
                this->playTurn(this->joueur, lineI + 1, colI + 1);
            }
            break;
        default:
            this->redraw = false;
            break;
    }

    this->keyCodePrev = keyCode;
}

void Grille::playTurn(char joueur, int line, int col) {
    this->cases[line-1][col-1]->setJoueur(joueur);
    if (this->joueur == J1) {
        this->joueur = J2;
    } else {
        this->joueur = J1;
    }
    this->checkWinner();
}