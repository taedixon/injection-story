#include "stdafx.h"

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
