#include "stdafx.h"
#include "npc.h"
#include "map.h"
#include "util.h"
#include "player.h"
#include "circuit.h"



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

void NPC_circuit_solver(CS_ENTITY* self) {
	int i, x, y;
	switch (self->scriptState) {
	case 1: // initialize circuit network
		resetCircuitNet();
		for (i = 0; i < 0x200; i++) {
			CS_ENTITY* npc = &CS_npcTable[i];
			if (npc->inUse && npc->flagId == self->flagId) {
				//add all elements to the circuit
				int xTile = npc->xPos / CS_SUBPX / 16;
				int yTile = npc->yPos / CS_SUBPX / 16;
				int type = getPxa(xTile, yTile);
				createCircuitElement(xTile, yTile, type);
			}
		}
		connectAllInputs();
	case 2: // run grid check
		checkGates();
		break;
	case 20: // free data
		break;
	default: // do nothing
		break;
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
