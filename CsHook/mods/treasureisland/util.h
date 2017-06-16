
void capMagnitude(int* x, int* y, int mag);
void vecFromAngle(int* x, int* y, int mag, double angle);
int getDirection(int x, int y);
void drawRect(RECT* r, int col);

enum DIRECTION
{
	WEST, NORTH, EAST, SOUTH
};
