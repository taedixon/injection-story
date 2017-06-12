#include "stdafx.h"
#include <cavestory.h>

#include "hooks.h"
#include "map.h"
#include "player.h"

void hook_treasureisland() {
	CS_oldmapdata[0] = (int)&loadMap;
	CS_oldmapdata[1] = (int)&freeMap;
	CS_oldmapdata[2] = (int)&drawStageBack;
	CS_oldmapdata[3] = (int)&drawStageFront;
	CS_oldmapdata[4] = (int)&control_topdown;
	CS_oldmapdata[5] = (int)&playerCalcFrame;
}
