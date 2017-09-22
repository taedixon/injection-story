#include "stdafx.h"
#include <cavestory.h>

#include "hooks.h"
#include "map.h"
#include "player.h"
#include "title.h"
#include "weapon.h"
#include "tsc.h"
#include "npc.h"
#include "util.h"
#include "boss.h"

void hook_treasureisland() {
	CS_oldmapdata[0] = (int)&loadMap;
	CS_oldmapdata[1] = (int)&freeMap;
	CS_oldmapdata[2] = (int)&drawStageBack;
	CS_oldmapdata[3] = (int)&drawStageFront;
	CS_oldmapdata[4] = (int)&control_topdown;
	CS_oldmapdata[5] = (int)&playerCalcFrame;
	CS_oldmapdata[6] = (int)&drawPlayer;
	CS_oldmapdata[7] = (int)&playerCheckTiles;
	CS_oldmapdata[8] = (int)&doTitle;
	CS_oldmapdata[9] = (int)bulletFuncs;
	CS_oldmapdata[10] = (int)&drawBullet;
	CS_oldmapdata[11] = (int)&weapon_shovel;
	CS_oldmapdata[12] = (int)&loadPxa;
	CS_oldmapdata[13] = (int)&getPxa;
	CS_oldmapdata[14] = (int)&drawPlayerArms;
	CS_oldmapdata[15] = (int)&drawPlayerLife;
	CS_oldmapdata[16] = (int)&tscHook;
	CS_oldmapdata[17] = (int)&drawMapName;
	CS_oldmapdata[18] = (int)&playerAct;
	CS_oldmapdata[19] = (int)&getCoin;
	CS_oldmapdata[20] = (int)&createNumObj;
	CS_oldmapdata[21] = (int)&isHitPlayer;

	
	// entities
	//beam that you hit with shovel
	CS_npcFuncTable[9] = &NPC_beamBall;
	// runs event on touch, will not run event again until player leaves hitbox and reenters
	CS_npcFuncTable[19] = &NPC_touchTrigger;
	// runs its event when the player digs on it; alt direction to require 4 digs before event
	CS_npcFuncTable[22] = &NPC_digSpot;
	//entrance to dungeons
	CS_npcFuncTable[23] = &NPC_dungeonEntrance;
	// it's the boat
	CS_npcFuncTable[29] = &NPC_boat;
	//launches the player into the air
	CS_npcFuncTable[39] = &NPC_springpad;
	CS_npcFuncTable[41] = &NPC_springpadFX;
	//default: U/D trigger (extends infinitely horizontal)
	//alt direction: L/R trigger (extends infinitely vertical)
	//state 1: player is left/above trigger
	//state 2: player is right/under trigger
	CS_npcFuncTable[43] = &NPC_boundsTrigger;
	//it's a pot
	CS_npcFuncTable[78] = &NPC_pot;
	//it's part of a pot
	CS_npcFuncTable[79] = &NPC_potShard;

	CS_npcFuncTable[80] = &NPC_gravekeeper;

	CS_npcFuncTable[97] = &NPC_fanUp;

	//runs an event when it's hit
	//direction sets colour
	CS_npcFuncTable[100] = &NPC_hitSwitch;
	CS_npcFuncTable[101] = &NPC_pedestalItem;

	CS_npcFuncTable[284] = &NPC_finalBoss;
	CS_npcFuncTable[285] = &NPC_tennisBall;
	CS_npcFuncTable[286] = &NPC_pleaseHitThis;
	CS_npcFuncTable[288] = &NPC_splitFocus;
	CS_npcFuncTable[289] = &NPC_shockwave;
	CS_npcFuncTable[290] = &NPC_simpleProjectile;

	CS_npcFuncTable[360] = &NPC_treasureRain;


	treasureActual = (int*)&CS_flagdata[996];
}
