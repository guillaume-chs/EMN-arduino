#include "U8glib/src/U8glib.h"

#define DAMIEN "Damien"
#define J1		'X'
#define J2		'O'
#define JNONE	'.'
#define CASE_SELECTED '_'

#define CASE_W	25
#define CASE_H	13
#define LINE_H	1
#define LINE_W 	1


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
	
	int getX(){
		return this->x;
	}
	int getY(){
		return this->y;
	}
	char getJoueur() {
		return this->joueur;
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
	
	void buildGrid(U8GLIB_NHD_C12864 *u8g, int xInit, int yInit);
	/*char *checkWinner() {

	}*/

public:
	Grille() {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				this->cases[i][j] = new Case(i+1, j+1);
			}
		}
		this->selected = this->cases[0][0];
	}
	~Grille() {
		delete this->cases;
	}
	

	void draw(U8GLIB_NHD_C12864 *u8g, int xInit, int yInit) {
		this->buildGrid(u8g, xInit, yInit);
		
		int yCoord;
		int xCoord;

		for (int i = 0; i < 3; i++) {
			yCoord = yInit + i*(CASE_H + LINE_H) + CASE_H/3;
			xCoord = xInit + CASE_W/2 - 1;

			for (int j = 0; j < 3; j++) {
				if (this->cases[i][j]->equals(this->selected)) {
					// u8g->drawStr(xCoord, yCoord, "_");
				} else {
					u8g->drawStr(xCoord, yCoord, this->cases[i][j]->print());
				}

				xCoord += (CASE_W + LINE_W);
			}
		}
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