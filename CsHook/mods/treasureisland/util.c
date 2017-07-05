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
