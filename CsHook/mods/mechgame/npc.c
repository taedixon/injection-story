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
	int i;
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
		self->scriptState = 99;
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
		toggleSwitch(xTile, yTile);
	}
}

void NPC_crawler(CS_ENTITY* self) {
	/*
		[ 0 3 6 ]
		[ 1 * 7 ]
		[ 2 5 8 ]
	*/
	const int boost = 0x300;
	const int wallfriction = 0x20;
	const int movewait = 20;
	unsigned char nearbyTiles[9];
	const int gravity = 0x60;
	const int cornerAdjust = CS_SUBPX;
	int i = 0;
	int xTile, yTile;
	int* moveAxis;
	int* wallAxis;
	int* movePos;
	int boostAmount;
	int signedGravity;
	int bonkFlag;
	int stillOnWallFlag;
	int bonkState;
	int flopState;
	char flipsSpeed;
	if (self->scriptState == 0) {
		// 0: initialize
		/*
		[ 0 3 6 ]
		[ 1 * 7 ]
		[ 2 5 8 ]
		*/
		xTile = self->xPos / CS_SUBPX / 16;
		yTile = self->yPos / CS_SUBPX / 16;
		for (int x = -1; x <= 1; x++) {
			for (int y = -1; y <= 1; y++) {
				nearbyTiles[i] = getPxa(xTile + x, yTile + y) == 0x41;
				i++;
			}
		}
		if (nearbyTiles[1]) {
			self->scriptState = self->direction ? 7 : 1;
			self->xPos = (xTile)* 16 * CS_SUBPX - (8 * CS_SUBPX);
			self->collision |= CS_TILEFLAG_HITLEFT;
		} else if (nearbyTiles[3]) {
			self->scriptState = self->direction ? 8 : 2;
			self->yPos = (yTile)* 16 * CS_SUBPX - (8 * CS_SUBPX);
			self->collision |= CS_TILEFLAG_HITROOF;
		} else if (nearbyTiles[7]) {
			self->scriptState = self->direction ? 5 : 3;
			self->xPos = xTile * 16 * CS_SUBPX + (8 * CS_SUBPX);
			self->collision |= CS_TILEFLAG_HITRIGHT;
		} else if (nearbyTiles[5]) {
			self->scriptState = self->direction ? 6 : 4;
			self->yPos = yTile * 16 * CS_SUBPX + (8 * CS_SUBPX);
			self->collision |= CS_TILEFLAG_HITFLOOR;
		}
	}
	switch (self->scriptState) {
	case 0:
		break;
	case 1:
		// 1: crawl up CW
		moveAxis = &self->yVel;
		wallAxis = &self->xVel;
		movePos = &self->yPos;
		boostAmount = -boost;
		signedGravity = -gravity;
		bonkFlag = CS_TILEFLAG_HITROOF;
		stillOnWallFlag = CS_TILEFLAG_HITLEFT;
		bonkState = 2;
		flopState = 4;
		flipsSpeed = 0;
		break;
	case 2:
		// 2: crawl right CW
		moveAxis = &self->xVel;
		wallAxis = &self->yVel;
		movePos = &self->xPos;
		boostAmount = boost;
		signedGravity = -gravity;
		bonkFlag = CS_TILEFLAG_HITRIGHT;
		stillOnWallFlag = CS_TILEFLAG_HITROOF;
		bonkState = 3;
		flopState = 1;
		flipsSpeed = 1;
		break;
	case 3:
		// 3: crawl down CW
		moveAxis = &self->yVel;
		wallAxis = &self->xVel;
		movePos = &self->yPos;
		boostAmount = boost;
		signedGravity = gravity;
		bonkFlag = CS_TILEFLAG_HITFLOOR;
		stillOnWallFlag = CS_TILEFLAG_HITRIGHT;
		bonkState = 4;
		flopState = 2;
		flipsSpeed = 0;
		break;
	case 4:
		// 2: crawl left CW
		moveAxis = &self->xVel;
		wallAxis = &self->yVel;
		movePos = &self->xPos;
		boostAmount = -boost;
		signedGravity = gravity;
		bonkFlag = CS_TILEFLAG_HITLEFT;
		stillOnWallFlag = CS_TILEFLAG_HITFLOOR;
		bonkState = 1;
		flopState = 3;
		flipsSpeed = 1;
		break;
	case 5:
		// 1: crawl up CCW
		moveAxis = &self->yVel;
		wallAxis = &self->xVel;
		movePos = &self->yPos;
		boostAmount = -boost;
		signedGravity = gravity;
		bonkFlag = CS_TILEFLAG_HITROOF;
		stillOnWallFlag = CS_TILEFLAG_HITRIGHT;
		bonkState = 8;
		flopState = 6;
		flipsSpeed = 1;
		break;
	case 6:
		// 2: crawl right CCW
		moveAxis = &self->xVel;
		wallAxis = &self->yVel;
		movePos = &self->xPos;
		boostAmount = boost;
		signedGravity = gravity;
		bonkFlag = CS_TILEFLAG_HITRIGHT;
		stillOnWallFlag = CS_TILEFLAG_HITFLOOR;
		bonkState = 5;
		flopState = 7;
		flipsSpeed = 0;
		break;
	case 7:
		// 3: crawl down CCW
		moveAxis = &self->yVel;
		wallAxis = &self->xVel;
		movePos = &self->yPos;
		boostAmount = boost;
		signedGravity = -gravity;
		bonkFlag = CS_TILEFLAG_HITFLOOR;
		stillOnWallFlag = CS_TILEFLAG_HITLEFT;
		bonkState = 6;
		flopState = 8;
		flipsSpeed = 1;
		break;
	case 8:
		// 4: crawl left CCW
		moveAxis = &self->xVel;
		wallAxis = &self->yVel;
		movePos = &self->xPos;
		boostAmount = -boost;
		signedGravity = -gravity;
		bonkFlag = CS_TILEFLAG_HITLEFT;
		stillOnWallFlag = CS_TILEFLAG_HITROOF;
		bonkState = 7;
		flopState = 5;
		flipsSpeed = 0;
		break;
	}
	// common to patrol states
	if (self->scriptState >= 1 && self->scriptState <= 8) {

		if (!self->scriptTimer) {
			*moveAxis = boostAmount;
			self->scriptTimer = movewait;
		}
		if (!((*moveAxis ^ boostAmount) & 0x80000000)) {
			// if our velocity in the motion axis has the same sign
			// as our boost velocity
			if (*moveAxis < 0) {
				*moveAxis += wallfriction;
			} else {
				*moveAxis -= wallfriction;
			}
			// if that sign has then changed, set velocity to 0
			if ((*moveAxis ^ boostAmount) & 0x80000000) *moveAxis = 0;
		}
		*wallAxis = signedGravity;
		if (self->directive) {
			// if we've recently detached from a wall,
			// wait until we hit our "floor" again before switching states.
			self->directive = !(self->collision & stillOnWallFlag);
		} else if (self->collision & bonkFlag) {
			// turn right
			self->scriptState = bonkState;
			if (bonkFlag & (CS_TILEFLAG_HITROOF | CS_TILEFLAG_HITFLOOR)) {
				// special behaviour; bonking the roof or floor resets velocity to 0
				// so, set the velocity back to the last known value
				*moveAxis = self->targetY;
			}
			*wallAxis = flipsSpeed ? *moveAxis : -*moveAxis;
		} else if (!(self->collision & stillOnWallFlag)) {
			// turn left
			self->scriptState = flopState;
			*wallAxis = flipsSpeed ? -*moveAxis : *moveAxis;
			// last ditch check to keep this from crashing lol
			if (*wallAxis != 0) {
				*wallAxis = CS_SUBPX * (*wallAxis / abs(*wallAxis));
			}
			if (*moveAxis > 0) {
				*movePos += cornerAdjust;
			} else {
				*movePos -= cornerAdjust;
			}
			*moveAxis = 0;
			self->directive = 1;
		}

		if (self->xVel == 0 || self->yVel == 0) {
			if (self->scriptTimer > 0) {
				self->scriptTimer--;
			}
		}
	}

	RECT frame = { 0, 0, 16, 16 };
	self->frameRect = frame;

	self->xPos += self->xVel;
	self->yPos += self->yVel;
	self->targetY = self->yVel;
}
