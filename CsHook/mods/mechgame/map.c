#include "stdafx.h"
#include <stdio.h>
#include <cavestory.h>

#include "map.h"
#include "util.h"
#include "player.h"

#define TILE_SIZE 16
#define VIEW_TILE_H 16
#define VIEW_TILE_W 21
#define TILESET_W 16

unsigned short** mapLayers;
unsigned char* pxaData = NULL;
int pxaLen;

int currentMapType = 0;
int tileAnimTimer = 0;
int physicalLayer = 2;
unsigned int nLayer = 0;
int fgSplit;

// return 1 for success, 0 for failure
int loadMap(char* mapName) {
	char mapBuffer[0x100];
	char header[4];
	unsigned short width, height;
	unsigned int numTiles;
	int result = 0;
	int layerType;
	unsigned int i;
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
			nLayer = 4;
			fgSplit = 2;
			physicalLayer = 2;
			*CS_mapTiles = malloc(numTiles);
			mapLayers = malloc(sizeof(unsigned short*)*nLayer);
			for (i = 0; i < nLayer; i++) {
				mapLayers[i] = malloc(numTiles * 2);
				fread(mapLayers[i], 2, numTiles, fileHandle);
			}
			//copy physical layer to the legacy buffer
			for (unsigned int i = 0; i < numTiles; i++) {
				(*CS_mapTiles)[i] = (unsigned char)mapLayers[physicalLayer][i];
			}
			fclose(fileHandle);
			result = 1;
			break;
		case 0x22:
			*CS_mapTiles = malloc(numTiles);
			fread(&nLayer, 4, 1, fileHandle);
			mapLayers = malloc(sizeof(unsigned short*)*nLayer);
			fgSplit = 0;
			for (i = 0; i < nLayer; i++) {
				mapLayers[i] = malloc(numTiles * 2);
				//set the physical layer for PXA values
				fread(&layerType, 4, 1, fileHandle);
				if (layerType == 2) {
					physicalLayer = i;
				}
				//check the first character in
				fread(header, 1, 1, fileHandle);
				if (header[0] == '*') {
					fgSplit = i;
				}
				// ignore the (rest of the) layer name, because we won't use it.
				fseek(fileHandle, 0x1F, SEEK_CUR);
				fread(mapLayers[i], 2, numTiles, fileHandle);
			}
			//copy physical layer to the legacy buffer
			for (unsigned int i = 0; i < numTiles; i++) {
				(*CS_mapTiles)[i] = (unsigned char)mapLayers[physicalLayer][i];
			}
			fclose(fileHandle);
			result = 1;

		}
		
		fclose(fileHandle);
	}

	return result;
}

int getPhysicalLayer() {
	return physicalLayer;
}

char loadPxa(char* mapName) {
	char nameBuf[0x100];
	FILE* fileHandle;

	free(pxaData);
	pxaData = NULL;

	sprintf(nameBuf, "%s\\%s", CS_dataDirPath, mapName);
	fileHandle = fopen(nameBuf, "rb");
	if (fileHandle) {
		fseek(fileHandle, 0, SEEK_END);
		pxaLen = ftell(fileHandle);
		fseek(fileHandle, 0, SEEK_SET);

		pxaData = malloc(pxaLen);
		fread(pxaData, 1, pxaLen, fileHandle);
		fclose(fileHandle);
		return 1;
	}
	return 0;
}

unsigned char getPxaLayer(unsigned int x, unsigned int y, unsigned int layer) {
	if (pxaData && x < *CS_mapWidth && y < *CS_mapHeight) {
		int loc = mapLayers[layer][x + *CS_mapWidth * y];
		if (loc < pxaLen) {
			return pxaData[loc];
		}
	}
	return 0;
}

unsigned char getPxa(unsigned int x, unsigned int y) {
	return getPxaLayer(x, y, physicalLayer);
}

void freeMap() {
	unsigned int i;
	for (i = 0; i < nLayer; i++) {
		free(mapLayers[i]);
		mapLayers[i] = NULL;
	}
	free(mapLayers);
	*CS_mapTiles = NULL;
}

unsigned short getTile(unsigned int x, unsigned int y, unsigned int layer) {
	return mapLayers[layer][x + *CS_mapWidth * y];
}

void setTile(unsigned int x, unsigned int y, unsigned int layer, unsigned int newType) {
	int tilePos = (y * *CS_mapWidth) + x;
	if (tilePos < (*CS_mapWidth * *CS_mapHeight) && layer < nLayer) {
		mapLayers[layer][tilePos] = newType;
	}
}

void _drawStage_legacy(int camX, int camY, BOOLEAN front) {
	//sanity check
	if (!*CS_mapTiles) 
		return;
	int startX = (camX / CS_SUBPX + 8) / TILE_SIZE;
	int startY = (camY / CS_SUBPX + 8) / TILE_SIZE;
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
			int screenX = (TILE_SIZE * x - 8) * CS_SUBPX;
			int screenY = (TILE_SIZE * y - 8) * CS_SUBPX;
			toScreenSpace(&screenX, &screenY, camX, camY);
			CS_putBitmap3(CS_fullScreenRect, screenX, screenY, &tileRect, CS_BM_TILESET);
			//NOTE - THIS FUNC DOES NOT DRAW STAR BLOCKS
		}
	}
}

void _drawStage(int camX, int camY, unsigned int layer) {
	int tframe = 0;
	int type;
	if (layer >= nLayer)
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
			//shortcut to save rendering time, tile 0 is always "empty"
			if (tileNum == 0) {
				continue;
			}
			type = getPxaLayer(x, y, layer);

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

			int screenX = (TILE_SIZE * x - 8) * CS_SUBPX;
			int screenY = (TILE_SIZE * y - 8) * CS_SUBPX;
			toScreenSpace(&screenX, &screenY, camX, camY);
			CS_putBitmap3(CS_fullScreenRect, screenX, screenY, &tileRect, CS_BM_TILESET);

			/*
			//DEBOOG
			RECT tileHitbox = {
				(x*TILE_SIZE + 2 - (TILE_SIZE / 2))*CS_SUBPX,
				(y*TILE_SIZE - (TILE_SIZE / 2))*CS_SUBPX,
				((x)*TILE_SIZE - 2 + (TILE_SIZE / 2))*CS_SUBPX,
				((y)*TILE_SIZE)*CS_SUBPX
			};

			RECT screenRect = tileHitbox;
			toScreenSpace(&screenRect.left, &screenRect.top, camX, camY);
			toScreenSpace(&screenRect.right, &screenRect.bottom, camX, camY);
			drawRect(&screenRect, 0xFF00FF);
			//DEBOOG
			*/

		}
	}
}

void drawStageBack(int cameraX, int cameraY) {
	int i;
	if (++tileAnimTimer >= 40320) {
		tileAnimTimer = 0;
	}
	switch (currentMapType) {
	case 0x10:
		_drawStage_legacy(cameraX, cameraY, FALSE);
		break;
	case 0x21:
	case 0x22:
		for (i = 0; i < fgSplit; i++) {
			_drawStage(cameraX, cameraY, i);
		}
		break;
	}
}

void drawStageFront(int cameraX, int cameraY) {
	unsigned int i;
	switch (currentMapType) {
	case 0x10:
		_drawStage_legacy(cameraX, cameraY, TRUE);
		break;
	case 0x21:
	case 0x22:
		for (i = fgSplit; i < nLayer; i++) {
			_drawStage(cameraX, cameraY, i);
		}
		break;
	}
}

int hitTile_oneway(int x, int y) {
	if (!(PrevTileFlags & 0x38) && (*CS_keyHeld & *CS_keyDown) && !(*CS_playerStateFlags & 2)) {
		// don't collide with this tile if the player was 
		// airborne in the previous frame and is holding down
		//and can control their player
		return 0;
	}
	if (*CS_playerYvel > 0) {
		//only can collide while moving downwards;
		RECT playerHitbox = {
			*CS_playerX - CS_playerHitRect->left,
			//we only want to consider a small hitbox around the player's feet, not their whole shape
			*CS_playerY + CS_playerHitRect->bottom - CS_SUBPX*2,
			*CS_playerX + CS_playerHitRect->right,
			*CS_playerY + CS_playerHitRect->bottom
		};
		RECT tileHitbox = {
			(x*TILE_SIZE + 2 - (TILE_SIZE / 2))*CS_SUBPX,
			(y*TILE_SIZE - (TILE_SIZE / 2))*CS_SUBPX,
			((x)*TILE_SIZE - 2 + (TILE_SIZE / 2))*CS_SUBPX,
			((y)*TILE_SIZE)*CS_SUBPX
		};
		
		if (intersect(&playerHitbox, &tileHitbox)) {
			*CS_playerYvel = 0x80;
			*CS_playerY = tileHitbox.top - CS_playerHitRect->bottom;
			return 8;
		}
	}
	return 0;
}
