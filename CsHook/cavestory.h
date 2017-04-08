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
