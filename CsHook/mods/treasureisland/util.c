#include "stdafx.h"
#include "cavestory.h"
#include "util.h"


double distance(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void capMagnitude(int* x, int* y, int magnitude) {
	double currentMag = distance(0, 0, *x, *y);
	if (currentMag > magnitude) {
		double angle = atan2(*y, *x);
		*x = (int)(cos(angle) * magnitude);
		*y = (int)(sin(angle) * magnitude);
	}
}

void vecFromAngle(int* x, int* y, int magnitude, double angle) {
	*x = (int)(cos(angle) * magnitude);
	*y = (int)(sin(angle) * magnitude);
}

enum DIRECTION getDirection(int x, int y) {
	char xMag = abs(x) > abs(y);
	if (x < -50 && xMag) {
		return WEST;
	}
	if (x > 50 && xMag) {
		return EAST;
	}
	if (y < 0) {
		return NORTH;
	}
	return SOUTH;
}

void drawRect(RECT* fr, int col) {
	RECT fr2;
	//use this to draw a hollow box (bad yeah)
	fr2 = *fr;
	fr2.right = fr2.left + 1;
	CS_fillRect(&fr2, col);

	fr2 = *fr;
	fr2.bottom = fr2.top + 1;
	CS_fillRect(&fr2, col);

	fr2 = *fr;
	fr2.left = fr2.right - 1;
	CS_fillRect(&fr2, col);

	fr2 = *fr;
	fr2.top = fr2.bottom - 1;
	CS_fillRect(&fr2, col);
}


void toScreenSpace(int* x, int* y, int cam_x, int cam_y) {
	*x &= -CS_SUBPX;
	*y &= -CS_SUBPX;
	*x -= cam_x;
	*y -= cam_y;
	*x /= CS_SUBPX;
	*y /= CS_SUBPX;
}



void drawTinyNumber(unsigned long value, int nDigit, int x, int y) {
	const int digitW = 4;
	const int digitH = 5;
	int factor;
	int val;
	int digitOffset;
	RECT digitRect;
	if (nDigit <= 0) {
		// draw just enough digits to hold the number
		factor = 1;
		while (value / factor > 0 && nDigit < 12) {
			nDigit++;
			factor *= 10;
		}
	}
	digitRect.top = 120;
	digitRect.bottom = digitRect.top + digitH;
	digitOffset = (digitW + 1) * nDigit;
	while (nDigit > 0) {
		val = (int)(value / pow(10, nDigit - 1));
		val %= 10;
		digitRect.left = 80 + val*digitW;
		digitRect.right = digitRect.left + digitW;
		CS_putBitmap3(CS_fullScreenRect, x + digitOffset - nDigit*(digitW + 1), y, &digitRect, CS_BM_TEXTBOX);
		nDigit--;
	}
}

void createNumObj(int x, int y, int amount) {

}

char intersect(RECT* r1, RECT* r2) {
	if (r1->right < r2->left ||
		r1->bottom < r2->top ||
		r1->left > r2->right ||
		r1->top > r2->bottom) {
		return 0;
	} else {
		// eliminating all possibilities for the hitboxes to be disjoint,
		// the only possibility that remains is that they overlap.
		return 1;
	}
}
