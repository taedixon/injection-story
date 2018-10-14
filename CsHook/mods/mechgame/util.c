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
	*x /= CS_SUBPX;
	*y /= CS_SUBPX;
	*x -= cam_x / CS_SUBPX;
	*y -= cam_y / CS_SUBPX;
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

void createNumObj(int* x, int* y, int amount) {
	const int DIGIT_W = 8;

	RECT digitRect;
	CS_NUM_OBJ* nums = CS_numbers;
	int i;
	int cNum;
	int isNegative = 0;
	int factor;
	int nDigit = 0;

	for (i = 0; i < 16; i++) {
		if (nums[i].isActive && nums[i].targetX == x && !((nums[i].value ^ amount) & 0x80000000)) {
			//if the number is tracking the same target and has the same sign
			break;
		}
	}
	if (i == 16) {
		cNum = *CS_lastUsedNumSlot;
		*CS_lastUsedNumSlot += 1;
		if (*CS_lastUsedNumSlot == 16) {
			*CS_lastUsedNumSlot = 0;
		}
		nums[cNum].field_14 = 0;
		nums[cNum].field_c = 0;
		nums[cNum].value = amount;
	} else {
		cNum = i;
		nums[cNum].field_14 = 32;
		nums[cNum].value += amount;
		amount = nums[cNum].value;
	}
	if (amount < 0) {
		isNegative = 1;
		amount = -amount;
	}
	//figure out how many digits there will be
	factor = 1;
	while (amount / factor > 0 && nDigit < 12) {
		nDigit++;
		factor *= 10;
	}

	//setup the object's framerect etc
	nums[cNum].isActive = 1;
	nums[cNum].targetX = x;
	nums[cNum].targetY = y;
	nums[cNum].frameRect.left = 0;
	nums[cNum].frameRect.top = DIGIT_W * cNum;
	nums[cNum].frameRect.right = (nDigit + 1) * DIGIT_W;
	nums[cNum].frameRect.bottom = DIGIT_W * (cNum + 1);

	//clear the drawing area
	CS_fillSurfaceRect(&nums[cNum].frameRect, 0, CS_BM_NUMBERBUF);

	//draw the plus/minus
	digitRect.top = 48;
	digitRect.bottom = 56;
	digitRect.left = 32;
	if (isNegative) {
		digitRect.left += DIGIT_W;
	}
	digitRect.right = digitRect.left + 8;
	CS_surfaceBlt(0, nums[cNum].frameRect.top, &digitRect,  CS_BM_NUMBERBUF, CS_BM_TEXTBOX);

	//draw the actual numbers
	digitRect.top = 56;
	if (isNegative) {
		digitRect.top += DIGIT_W;
	}
	digitRect.bottom = digitRect.top + DIGIT_W;
	int digitOffset = DIGIT_W * (nDigit+1);
	while (nDigit > 0) {
		i = (int)(amount / pow(10, nDigit - 1));
		i %= 10;
		digitRect.left = i*DIGIT_W;
		digitRect.right = digitRect.left + DIGIT_W;
		CS_surfaceBlt(digitOffset - nDigit*(DIGIT_W), nums[cNum].frameRect.top, &digitRect, CS_BM_NUMBERBUF, CS_BM_TEXTBOX);
		nDigit--;
	}
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
