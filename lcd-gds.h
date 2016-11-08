#include "U8glib/src/U8glib.h"

#define J1		'X'
#define J2		'O'
#define JNONE	'.'
#define CASE_SELECTED '_'

#define CASE_W	25
#define CASE_H	13
#define LINE_H	1
#define LINE_W 	1
#define FONT_H 	9


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

	char *checkWinner() {

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
	bool needsRedraw()		{ return this->redraw; }
	void drawed()			{ this->redraw = false; }
	
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
			else		this->selected->setX(x-1);
			break;
		case JSK_RIGHT:
			if (x == 3) this->selected->setX(1);
			else		this->selected->setX(x+1);
			break;
		case JSK_TOP:
			if (y == 1) this->selected->setY(3);
			else		this->selected->setY(y-1);
			break;
		case JSK_BOTTOM:
			if (y == 3) this->selected->setY(1);
			else		this->selected->setY(y+1);
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