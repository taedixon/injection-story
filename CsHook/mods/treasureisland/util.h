
void capMagnitude(int* x, int* y, int mag);
void vecFromAngle(int* x, int* y, int mag, double angle);
int getDirection(int x, int y);
void drawRect(RECT* r, int col);
void toScreenSpace(int* x, int* y, int cam_x, int cam_y);

enum DIRECTION
{
	WEST, NORTH, EAST, SOUTH
};
