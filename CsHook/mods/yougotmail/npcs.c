#include "stdafx.h"
#include "cavestory.h"
#include "d3dscene.h"
#include "npcs.h"

void npc_wiggle(CS_ENTITY* This) {
	for (int i = 0; i < 0x200; i++) {
		CS_ENTITY* npc = &CS_npcTable[i];
		if (npc->inUse && npc->npcID != 0) {
			npc->xPos += CS_randInt(-1, 1) * 0x200;
		}
	}
}

void downshift(CS_ENTITY* This) {
	*CS_orgTempo *= 2;
	// we call these functions to reset the timer used for org playback
	// this way our changes to the tempo will be picked up immediately
	CS_QuitMMTimer();
	CS_PlayOrganyaMusic();

	for (int track = 0; track < 0x10; track++) {
		//0x4a4E18 pointer to ORGdata
		//0x4a4E18+x018 trackinfo array [0x0C*16]
		CS_NOTEINFO* notePointer = *(CS_NOTEINFO**)(0x4A4E30 + (track * 0xC));
		while (notePointer) {
			// lower the pitch of each note by a random amt
			if (notePointer->pitch != 0xFF) {
				notePointer->pitch += CS_randInt(-5,-2);
			}
			notePointer = notePointer->nextNote;
		}
	}
	This->inUse = 0;
}

void music_fasterer(CS_ENTITY* This) {
	if (*CS_orgTempo > 0) {
		if (*CS_orgTempo <= 50) {
			This->inUse = 0;
		}
		// increase the tempo by 1 every second
		if (++This->scriptState > 40) {
			This->scriptState = 0;
			*CS_orgTempo -= 2;
			CS_QuitMMTimer();
			CS_PlayOrganyaMusic();
			// fastness limit
		}
	}
}

void door2(CS_ENTITY* This) {
	if (This->direction) {
		This->frameRect.left = 192;
		This->frameRect.top = 112;
		This->frameRect.right = 208;
		This->frameRect.bottom = 136;
	}
	else {
		This->frameRect.left = 224;
		This->frameRect.top = 16;
		This->frameRect.right = 240;
		This->frameRect.bottom = 40;
	}

	if (This->scriptState) {
		This->scriptTimer++;
		if (This->scriptTimer > 300) {
			This->inUse = 0;
		}
		This->yVel -= 10;
		This->yPos += This->yVel;
	}
}

void bgmode_randomizer(CS_ENTITY* This) {
	if (--This->scriptTimer <= 0) {
		This->scriptTimer = CS_randInt(20, 200);
		*CS_bgMode = CS_randInt(1, 7);
	}
}

unsigned char* mapdataCopy;
void wacky_tiles(CS_ENTITY* This) {
	int mapdataLen = *CS_mapWidth * *CS_mapHeight;
	if (This->scriptState == 0) {
		// initialize the copy
		if (mapdataCopy) {
			free(mapdataCopy);
		}
		mapdataCopy = malloc(mapdataLen);
		memcpy(mapdataCopy, *CS_mapTiles, mapdataLen);
		This->scriptState = 1;
	}
	for (int i = 0; i < mapdataLen; i++) {
		unsigned char type = CS_pxaData[mapdataCopy[i]];
		if (type == 0x20) {
			(*CS_mapTiles)[i] = CS_randInt(0, 255);
		}
	}
}

void loop_top(CS_ENTITY* This) {
	if (*CS_playerY >(This->yPos + 0x2000)) {
		*CS_playerY = -0x2000;
	}
}

void slowerator(CS_ENTITY* This) {
	int speedMod;
	if (*CS_playerXvel > 0) {
		speedMod = -1;
	} else {
		speedMod = 1;
	}
	speedMod *= This->scriptState;

	if (This->scriptState != 0) {
		if (*CS_playerXvel > This->direction) {
			*CS_playerXvel = This->direction;
		} else if (*CS_playerXvel < -This->direction) {
			*CS_playerXvel = -This->direction;
		}
	}

	*CS_playerXvel += speedMod;
}

void text_sound_weird(CS_ENTITY* This) {
	if (This->scriptState == 0) {
		//init
		This->directive = CS_soundPointers[2];
	}
	if (This->direction == 0) {
		//reset the sound
		CS_soundPointers[2] = This->directive;
	}
	else {
		CS_soundPointers[2] = CS_soundPointers[This->direction];
	}
}

void big_head(CS_ENTITY* This) {
	This->frameRect.left = 0;
	This->frameRect.top = 0;
	This->frameRect.right = 176;
	This->frameRect.bottom = 224;

	if (This->scriptState == 0) {
		This->scriptState = 1;
		This->xVel = This->xPos;
		This->yVel = This->yPos;
	} else if (This->scriptState == 10) {

		initD3D(*CS_appWinHandle);
		int i = 0;
		while (++i < 250) {
			CS_peek_message();
			render_frame();
			Sleep(10);
		}
		cleanD3D();
		This->inUse = 0;
	}
	This->xPos = This->xVel + CS_randInt(-400, 400);
	This->yPos = This->yVel + CS_randInt(-400, 400);
}
