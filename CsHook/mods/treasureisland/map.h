

int loadMap(char* mapName);
char loadPxa(char* mapName);
unsigned char getPxa(unsigned int x, unsigned int y);
void freeMap();

void drawStageBack(int cameraX, int cameraY);
void drawStageFront(int cameraX, int cameraY);

int hitTile_oneway(int x, int y);
