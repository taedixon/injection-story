#include "stdafx.h"
#include "npc.h"
#include "map.h"
#include "util.h"
#include "player.h"

struct POINT {
	unsigned short x;
	unsigned short y;
};

#define POWERNET_SIZE 1024
POINT PowerNet[POWERNET_SIZE];

int isHitPlayer(CS_ENTITY* ent) {
	RECT playerHitbox = {
		*CS_playerX - CS_playerHitRect->left,
		*CS_playerY - CS_playerHitRect->top,
		*CS_playerX + CS_playerHitRect->right,
		*CS_playerY + CS_playerHitRect->bottom
	};
	RECT npcHitbox = {
		ent->xPos - ent->hitRect.left,
		ent->yPos - ent->hitRect.top,
		ent->xPos + ent->hitRect.right,
		ent->yPos + ent->hitRect.bottom
	};

	return intersect(&playerHitbox, &npcHitbox);
}

void NPC_touchTrigger(CS_ENTITY* self) {
	int hit = isHitPlayer(self);
	if (self->scriptState && !hit) {
		self->scriptState = 0;
	} else if (!self->scriptState && hit) {
		self->scriptState = 1;
		CS_runEvent(self->eventNum);
	}
}

void resetGrid(unsigned int layer) {
	for (int x = 0; x < *CS_mapWidth; x++) {
		for (int y = 0; y < *CS_mapHeight; y++) {
			unsigned char type = getPxaLayer(x, y, layer);
			if (type == 0x11 || type == 0x12) {
				unsigned short tile = getTile(x, y, layer);
				setTile(x, y, layer, tile + 0x10);
			}
		}
	}
}

int powerWire(unsigned int x, unsigned int y, unsigned int layer) {
	unsigned char type = getPxaLayer(x, y, layer);
	unsigned short tileId = getTile(x, y, layer);
	if (type == 0x46 || type == 0x45) {
		setTile(x, y, layer, tileId - 0x10);
		return 1;
	}
	return 0;
}

int checkGate(int gateX, int gateY, int gridLayer, int writePoint) {
	unsigned char gateType = getPxaLayer(gateX, gateY, gridLayer);
	int powerCount = 0;
	int outX = 0, outY = 0;
	for (int dx = -1; dx < 2; dx++) {
		for (int dy = -1; dy < 2; dy++) {
			int tile = getPxaLayer(gateX + dx, gateY + dy, gridLayer);
			if (tile == 0x12) {
				powerCount++;
			}
			if (tile == 0x45) {
				outX = gateX + dx;
				outY = gateY + dy;
			}
		}
	}
	if (!(outX | outY)) {
		return 0;
	}
	char powered = 0;
	switch (gateType) {
	case 0x91: //xor
		if (powerCount == 1) {
			powered = powerWire(outX, outY, gridLayer);
		}
		break;
	case 0x90: //and
		if (powerCount == 2) {
			powered = powerWire(outX, outY, gridLayer);
		}
		break;
	case 0x92: // NOR
		if (powerCount == 0) {
			powered = powerWire(outX, outY, gridLayer);
		}
		break;
	}
	if (powered) {
		PowerNet[writePoint].x = outX;
		PowerNet[writePoint].y = outY;
	}

	return powered;
}

int checkGates(int gridLayer, int writePoint) {
	int written = 0;
	for (int i = 0; i < 0x200; i++) {
		CS_ENTITY* npc = &CS_npcTable[i];
		if (npc->inUse && npc->npcID == 70) {
			//check this gate
			int xTile = npc->xPos / CS_SUBPX / 16;
			int yTile = npc->yPos / CS_SUBPX / 16;
			if (checkGate(xTile, yTile, gridLayer, writePoint + written)) {
				written++;
			}
		}
	}
	return written;
}

int solvePowerNet(int checkPoint, int writingPoint, int gridLayer) {
	POINT* checking = NULL;
	POINT* writing = &PowerNet[writingPoint];
	for (;;) {
		checking = &PowerNet[checkPoint];
		// quit looping if x and y are 0
		if (!(checking->x | checking->y)) {
			break;
		}
		if (powerWire(checking->x - 1, checking->y, gridLayer)) {
			writing->x = checking->x - 1;
			writing->y = checking->y;
			writing++;
			writingPoint++;
		}
		if (powerWire(checking->x + 1, checking->y, gridLayer)) {
			writing->x = checking->x + 1;
			writing->y = checking->y;
			writing++;
			writingPoint++;
		}
		if (powerWire(checking->x, checking->y - 1, gridLayer)) {
			writing->x = checking->x;
			writing->y = checking->y - 1;
			writing++;
			writingPoint++;
		}
		if (powerWire(checking->x, checking->y + 1, gridLayer)) {
			writing->x = checking->x;
			writing->y = checking->y + 1;
			writing++;
			writingPoint++;
		}
		checkPoint++;
	}
	return writingPoint;
}

void NPC_circuit_solver(CS_ENTITY* self) {
	if (self->scriptState == 0) {
		unsigned int layer = self->flagId;
		// circuit needs to be reassessed.
		memset(PowerNet, 0, sizeof(POINT)*POWERNET_SIZE);
		resetGrid(layer);
		int powerSourceWritten = 0;
		for (int i = 0; i < 0x200; i++) {
			CS_ENTITY* npc = &CS_npcTable[i];
			if (npc->inUse && npc->npcID == 102) {
				//add all power sources to the net
				int xTile = npc->xPos / CS_SUBPX / 16;
				int yTile = npc->yPos / CS_SUBPX / 16;
				PowerNet[powerSourceWritten].x = xTile;
				PowerNet[powerSourceWritten].y = yTile;
				powerSourceWritten++;
			}
		}
		for (;;) {
			powerSourceWritten = solvePowerNet(0, powerSourceWritten, layer);
			int gatesPowered = checkGates(layer, powerSourceWritten);
			if (!gatesPowered) {
				break;
			} else {
				powerSourceWritten += gatesPowered;
			}
		}

		self->scriptState = 1;
	}
}

void NPC_circuitSwitch(CS_ENTITY* self) {
	if (self->scriptState) {
		self->scriptState = 0;
		int xTile = self->xPos / CS_SUBPX / 16;
		int yTile = self->yPos / CS_SUBPX / 16;
		int layer = self->flagId;
		int switchStatus = getPxaLayer(xTile, yTile, layer);
		int tileId = getTile(xTile, yTile, layer);
		switch (switchStatus) {
		case 0x11: //lit closed circuit
			setTile(xTile, yTile, layer, tileId + 0x2F);
			break;
		case 0x10:
			setTile(xTile, yTile, layer, tileId - 0x2F);
		}
	}
}
