#include "stdafx.h"

typedef struct Entity {
	int inUse;		//+0x00
	int collision;	//+0x04
	int xPos;		//+0x08
	int yPos;		//+0x0C
	int xVel;		//+0x10
	int yVel;		//+0x14
	int alt_xVel;	//+0x18
	int alt_yVel;	//+0x1C
	int target_x;	//+0x20
	int target_y;	//+0x24
	int npcID;		//+0x28
	int entityID;	//+0x2C
	int eventNum;	//+0x30
	int tilesetID;	//+0x34
	int hurtSound;	//+0x38
	int deathSound;	//+0x3C
	int health;		//+0x40
	int xp;			//+0x44
	int size;		//+0x48
	int direction;	//+0x4C
	int flags;		//+0x50
	RECT frameRect;	//+0x54
	int frameTimer;	//+x064
	int frameNum;	//
	int objectTimer;
	int directive;
	int scriptState;
	int scriptTimer;
	RECT hitRect;
	RECT displayRect;
	int hitTrue;
	int damageTaken;
	int damage;
	struct Entity* parent;
} CS_ENTITY;

//size: 0x10
typedef struct NoteInfo {
	struct NoteInfo* previousNote;
	struct NoteInfo* nextNote;
	int beatNum;
	unsigned char len; //+C
	unsigned char pitch; //+D
	unsigned char vol; //+E
	unsigned char pan; //+F
} CS_NOTEINFO;

typedef struct {
	char tileset[32];
	char filename[32];
	int scrollType;
	char bgName[32];
	char npc1[32];
	char npc2[32];
	char bossNum;
	char mapName[35];
} CS_MAPDATA;

//VARs
extern HWND* CS_appWinHandle;
extern CS_ENTITY* CS_npcTable;
extern CS_MAPDATA* CS_mapdata;
extern unsigned short* CS_orgTempo;
extern int* CS_bgMode;
extern short* CS_mapWidth;
extern short* CS_mapHeight;
extern unsigned char** CS_mapTiles;
extern unsigned char* CS_pxaData;
extern int* CS_playerX;
extern int* CS_playerY;
extern int* CS_playerXvel;
extern int* CS_playerYvel;
extern int* CS_soundPointers;

//FUNCs
extern void(**CS_npcFuncTable)(CS_ENTITY*);
extern int(*CS_randInt)(int, int);
extern BOOL(*CS_peek_message)();
extern void(*CS_QuitMMTimer)();
extern void(*CS_PlayOrganyaMusic)();
extern unsigned char(*CS_GetTileType)(int, int);
