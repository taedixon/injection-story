
void capMagnitude(int* x, int* y, int mag);
void vecFromAngle(int* x, int* y, int mag, double angle);
int getDirection(int x, int y);
void drawRect(RECT* r, int col);
void toScreenSpace(int* x, int* y, int cam_x, int cam_y);
void drawTinyNumber(unsigned long value, int nDigit, int x, int y);
char intersect(RECT* r1, RECT* r2);

enum DIRECTION
{
	WEST, NORTH, EAST, SOUTH
};
