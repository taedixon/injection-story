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
	CS_oldmapdata[10] = (int)&drawBullet_debug;
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
	CS_npcFuncTable[19] = &NPC_touchTrigger;
	CS_npcFuncTable[22] = &NPC_digSpot;
	CS_npcFuncTable[29] = &NPC_boat;
	CS_npcFuncTable[43] = &NPC_boundsTrigger;
	CS_npcFuncTable[78] = &NPC_pot;
	CS_npcFuncTable[79] = &NPC_potShard;

	treasureActual = (int*)&CS_flagdata[996];
}
