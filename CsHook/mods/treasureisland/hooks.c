#include "stdafx.h"
#include <cavestory.h>

#include "hooks.h"
#include "map.h"
#include "player.h"
#include "title.h"
#include "weapon.h"
#include "tsc.h"

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
}
