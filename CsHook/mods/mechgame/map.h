

int loadMap(char* mapName);
char loadPxa(char* mapName);
unsigned char getPxaLayer(unsigned int x, unsigned int y, unsigned int layer);
unsigned char getPxa(unsigned int x, unsigned int y);
void freeMap();


void drawStageBack(int cameraX, int cameraY);
void drawStageFront(int cameraX, int cameraY);

int hitTile_oneway(int x, int y);

void setTile(unsigned int x, unsigned int y, unsigned int layer, unsigned int type);
unsigned short getTile(unsigned int x, unsigned int y, unsigned int layer);
int getPhysicalLayer();
