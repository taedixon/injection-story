#include "stdafx.h"
#include <cavestory.h>
#include "npcs.h"
#include "d3dscene.h"

#include "hooks.h"

void inject_NPCs() {
	CS_npcFuncTable[360] = &npc_wiggle;
	CS_npcFuncTable[358] = &downshift;
	CS_npcFuncTable[356] = &music_fasterer;
	CS_npcFuncTable[18] = &door2;
	CS_npcFuncTable[355] = &bgmode_randomizer;
	CS_npcFuncTable[353] = &wacky_tiles;
	CS_npcFuncTable[351] = &loop_top;
	CS_npcFuncTable[350] = &slowerator;
	CS_npcFuncTable[349] = &text_sound_weird;
	CS_npcFuncTable[299] = *big_head;
}

void inject_mapdata() {
	CS_mapdata[15].tileset[0] = 'B';
	CS_mapdata[15].bgName[2] = 'C';
	CS_mapdata[49].scrollType = 4;
}

void hijack_d3d() {
	// destroy existing dd7 configuration
	// dunno if this is actually necessary might want to keep it? ueh

	initD3D(*CS_appWinHandle);
	while (CS_peek_message()) {
		render_frame();
		Sleep(10);
	}
	cleanD3D();
}


void hook_yougotmail() {
	inject_NPCs();
	inject_mapdata();
}
