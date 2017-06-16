#include "stdafx.h"
#include <stdio.h>
#include <cavestory.h>

#include "map.h"

#define NUM_LAYER 4
#define PHYSICAL_LAYER 2
#define TILE_SIZE 16
#define VIEW_TILE_H 16
#define VIEW_TILE_W 21
#define TILESET_W 16

unsigned short* mapLayers[NUM_LAYER];

int currentMapType = 0;
int tileAnimTimer = 0;

// return 1 for success, 0 for failure
int loadMap(char* mapName) {
	char mapBuffer[0x100];
	char header[4];
	unsigned short width, height;
	unsigned int numTiles;
	int result = 0;
	int i;
	FILE* fileHandle;

	sprintf(mapBuffer, "%s/%s", CS_dataDirPath, mapName);
	fileHandle = fopen(mapBuffer, "rb");
	if (fileHandle) {

		fread(header, 1, 4, fileHandle);
		fread(&width, 2, 1, fileHandle);
		fread(&height, 2, 1, fileHandle);
		*CS_mapWidth = width;
		*CS_mapHeight = height;
		numTiles = width*height;


		freeMap();
		currentMapType = header[3];
		switch (header[3]) {
		case 0x10:
			// allocate
			*CS_mapTiles = malloc(numTiles);
			//read
			fread(*CS_mapTiles, 1, numTiles, fileHandle);
			fclose(fileHandle);
			result = 1;
			break;
		case 0x21:
			//allocate
			*CS_mapTiles = malloc(numTiles);
			for (i = 0; i < NUM_LAYER; i++) {
				mapLayers[i] = malloc(numTiles * 2);
				fread(mapLayers[i], 2, numTiles, fileHandle);
			}
			//copy physical layer to the legacy buffer
			for (unsigned int i = 0; i < numTiles; i++) {
				(*CS_mapTiles)[i] = (unsigned char)mapLayers[PHYSICAL_LAYER][i];
			}
			fclose(fileHandle);
			result = 1;
			break;
		}
		
		fclose(fileHandle);
	}

	return result;
}

void freeMap() {
	for (int i = 0; i < NUM_LAYER; i++) {
		free(mapLayers[i]);
		mapLayers[i] = NULL;
	}
	*CS_mapTiles = NULL;
}


void _drawStage_legacy(int camX, int camY, BOOLEAN front) {
	//sanity check
	if (!*CS_mapTiles) 
		return;
	int startX = (camX / 512 + 8) / TILE_SIZE;
	int startY = (camY / 512 + 8) / TILE_SIZE;
	RECT tileRect;
	for (int y = startY; y < startY + VIEW_TILE_H; y++) {
		for (int x = startX; x < startX + VIEW_TILE_W; x++) {
			int offset = x + *CS_mapWidth * y;
			int tileNum = *CS_mapTiles[offset];
			int type = CS_GetTileType(x, y);
			if (front && (type < 0x40 || type >= 0x80))
				continue;
			if (!front && type >= 0x20)
				continue;
			tileRect.left = TILE_SIZE * (tileNum % TILESET_W);
			tileRect.top = TILE_SIZE * (tileNum / TILESET_W);
			tileRect.right = tileRect.left + TILE_SIZE;
			tileRect.bottom = tileRect.top + TILE_SIZE;
			int screenX = TILE_SIZE * x - 8 - (camX / 512);
			int screenY = TILE_SIZE * y - 8 - (camY / 512);
			CS_putBitmap3(CS_fullScreenRect, screenX, screenY, &tileRect, CS_BM_TILESET);
			//NOTE - THIS FUNC DOES NOT DRAW STAR BLOCKS
		}
	}
}

void _drawStage(int camX, int camY, unsigned int layer) {
	int tframe = 0;
	if (layer >= NUM_LAYER)
		return;
	unsigned short* tileLayer = mapLayers[layer];
	if (!tileLayer)
		return;
	int startX = (camX / 512 + 8) / TILE_SIZE;
	int startY = (camY / 512 + 8) / TILE_SIZE;
	RECT tileRect;
	for (int y = startY; y < startY + VIEW_TILE_H; y++) {
		for (int x = startX; x < startX + VIEW_TILE_W; x++) {
			tframe = 0;
			int offset = x + *CS_mapWidth * y;
			unsigned int tileNum = tileLayer[offset];
			int type = CS_pxaData[tileNum];

			tileRect.left = TILE_SIZE * (tileNum % TILESET_W);
			tileRect.top = TILE_SIZE * (tileNum / TILESET_W);
			tileRect.right = tileRect.left + TILE_SIZE;
			tileRect.bottom = tileRect.top + TILE_SIZE;

			//calc tile animation
			if (type > 0x20 && type <= 0x40) {
				tframe = (tileAnimTimer / (type-0x20)) % 4;
			} else if ((type % 0x10) >= 8) {
				tframe = (tileAnimTimer / 16) % 4;
			}
			tileRect.top += tframe * TILE_SIZE;
			tileRect.bottom += tframe * TILE_SIZE;

			int screenX = TILE_SIZE * x - 8 - (camX / 512);
			int screenY = TILE_SIZE * y - 8 - (camY / 512);
			CS_putBitmap3(CS_fullScreenRect, screenX, screenY, &tileRect, CS_BM_TILESET);

		}
	}
}

void drawStageBack(int cameraX, int cameraY) {
	if (++tileAnimTimer >= 40320) {
		tileAnimTimer = 0;
	}
	switch (currentMapType) {
	case 0x10:
		_drawStage_legacy(cameraX, cameraY, FALSE);
		break;
	case 0x21:
		_drawStage(cameraX, cameraY, 0);
		_drawStage(cameraX, cameraY, 1);
		break;
	}
}

void drawStageFront(int cameraX, int cameraY) {
	switch (currentMapType) {
	case 0x10:
		_drawStage_legacy(cameraX, cameraY, TRUE);
		break;
	case 0x21:
		_drawStage(cameraX, cameraY, 2);
		_drawStage(cameraX, cameraY, 3);
		break;
	}
}
