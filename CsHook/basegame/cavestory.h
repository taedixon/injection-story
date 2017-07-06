
#pragma once
#include "stdafx.h"

#ifndef CAVESTORY_H
#define CAVESTORY_H

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

//size: 0x14
typedef struct {
	int id;
	int level;
	int energy;
	int maxAmmo;
	int curAmmo;
} CS_WEAPON;

//size: 0x80
typedef struct {
	int collision;
	int shotID;
	int flags;
	int inUse;
	int xPos;
	int yPos;
	int xVel;
	int yVel;
	int unk1;
	int unk2;
	int state;
	int unk3;
	int unk4;
	int frameID;
	int direction;
	RECT frameRect;
	int framesActive; // how many frames the bullet has been active
	int unk5;
	int maxFrame; // the max number of frames it can be alive
	int damage;
	int numHits;
	RECT hitRect;
	RECT sizeRect;
} CS_BULLET;

//VARs
//npc
extern CS_ENTITY* CS_npcTable;
//map
extern CS_MAPDATA* CS_mapdata;
extern int* CS_oldmapdata;
extern int* CS_bgMode;
extern unsigned short* CS_mapWidth;
extern unsigned short* CS_mapHeight;
extern unsigned char** CS_mapTiles;
extern unsigned char* CS_pxaData;
extern int* CS_globalWaterDepth;
//player
extern int* CS_playerDirection;
extern int* CS_playerFacingUp;
extern int* CS_playerFacingDown;
extern int* CS_playerFrameNum;
extern int* CS_playerEquipFlags;
extern int* CS_playerStateFlags;
extern int* CS_playerTileFlags;
extern int* CS_playerX;
extern int* CS_playerY;
extern int* CS_playerXvel;
extern int* CS_playerYvel;
extern int* CS_playerCamX;
extern int* CS_playerCamY;
extern int* CS_playerCamFocusX;
extern int* CS_playerCamFocusY;
extern RECT* CS_playerFrameRect;
extern RECT* CS_playerHitRect;
extern RECT* CS_playerSizeRect;
extern RECT* CS_playerWepRect;
extern CS_WEAPON* CS_playerWeapons;
extern int* CS_playerSelWeapon;
extern int* CS_playerCurrentHealth;
//sound
extern int* CS_soundPointers;
extern unsigned short* CS_orgTempo;
//system
extern HWND* CS_appWinHandle;
extern char* CS_dataDirPath;
extern RECT* CS_fullScreenRect;
extern int* CS_lastKeyHeld;
extern int* CS_keyHeld;
extern int* CS_keyPressed;
extern int* CS_controlMode;
extern int* CS_gameState;
//script
extern char** CS_scriptPointer;
extern int* CS_scriptOffset;
extern unsigned char* CS_tscState;
extern int* CS_currentFacepic;
//keys
extern int* CS_keyJump;
extern int* CS_keyShoot;
extern int* CS_keyNextWep;
extern int* CS_keyPrevWep;
extern int* CS_keyMenu;
extern int* CS_keyJump;
extern int* CS_keyLeft;
extern int* CS_keyUp;
extern int* CS_keyRight;
extern int* CS_keyDown;


//FUNCs
extern void(**CS_npcFuncTable)(CS_ENTITY*);
extern int(*CS_randInt)(int, int);
extern BOOL(*CS_flipScreen)(HWND);
extern void(*CS_QuitMMTimer)();
extern void(*CS_PlayOrganyaMusic)();
extern void(*CS_playSong)(int);
extern void(*CS_playSound)(int sfxid, int channel);
extern unsigned char(*CS_GetTileType)(int, int);
extern void(*CS_putBitmap3)(RECT*, int, int, RECT*, int);
extern void(*CS_fillRect)(RECT*, int);
extern void(*CS_updateKeys)();
extern int(*CS_escMenuLoop)(HWND window);
extern void(*CS_setMapName)(char* name);

extern void(*CS_createBullet)(int id, int x, int y, int direction);
extern void(*CS_createNPC)(int id, int x, int y, int xVel, int yVel, int direction, int unk, int slot);
extern void(*CS_createEffect)(int x, int y, int id, int mode);

extern int(*CS_playerHitSquareBlock)();
extern int(*CS_playerHitSlopeFloorA)();
extern int(*CS_playerHitSlopeFloorB)();
extern int(*CS_playerHitSlopeFloorC)();
extern int(*CS_playerHitSlopeFloorD)();
extern int(*CS_playerHitSlopeRoofA)();
extern int(*CS_playerHitSlopeRoofB)();
extern int(*CS_playerHitSlopeRoofC)();
extern int(*CS_playerHitSlopeRoofD)();
extern int(*CS_playerHitSpike)();
extern int(*CS_playerHitSpikeWater)();
extern int(*CS_playerHitWater)();
extern int(*CS_playerHitWindLeft)();
extern int(*CS_playerHitWindUp)();
extern int(*CS_playerHitWindRight)();
extern int(*CS_playerHitWindDown)();

//constants
#define CS_BM_TITLE 0
#define CS_BM_TILESET 2
#define CS_BM_ARMS 11
#define CS_BM_PLAYER 16

#define CS_SUBPX 0x200

#endif
