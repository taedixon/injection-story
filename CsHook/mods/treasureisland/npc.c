#include "stdafx.h"
#include "npc.h"
#include "util.h"
#include "player.h"

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

void NPC_boat(CS_ENTITY* self) {
	const int speedcap = 0x40;
	RECT boatRect = {
		0, 0, 482, 397
	};

	switch (self->scriptState) {
	case 0:
		self->scriptState = 1;
		self->yVel = speedcap;
	case 1:
		if (++self->scriptTimer < 2) break;
		self->scriptTimer = 0;
		self->yVel--;
		if (self->yVel <= -speedcap)
			self->scriptState = 2;
		break;
	case 2:
		if (++self->scriptTimer < 2) break;
		self->scriptTimer = 0;
		self->yVel++;
		if (self->yVel >= speedcap)
			self->scriptState = 1;
	}

	self->xPos += self->xVel;
	self->yPos += self->yVel;
	self->frameRect = boatRect;
}

void NPC_pot(CS_ENTITY* self) {
	// initialize the pot with a random graphic
	// and set its XP equal to its event #
	if (!self->scriptState) {
		self->scriptState = 1;
		self->xp = self->eventNum;

		int potGfx = CS_randInt(0, 3);
		self->frameRect.top = 88;
		self->frameRect.bottom = 120;
		self->frameRect.left = 256 + potGfx * 0x10;
		self->frameRect.right = self->frameRect.left + 0x10;
	}

	if (self->health < 1000) {
		//spawn 4x npc_potshard
		CS_killNpc(self, 1);
		int shards = CS_randInt(4, 6);
		for (int i = 0; i < shards; i++) {
			CS_createNPC(79, self->xPos, self->yPos, 0, 0, 0, 0, 0x100);
		}
	}
}

void NPC_potShard(CS_ENTITY* self) {
	if (!self->scriptState) {
		self->scriptState = 1;
		self->xVel = CS_randInt(-1000, 1000);
		self->yVel = CS_randInt(-1500, -2500);
		self->directive = CS_randInt(0, 4);
		self->frameNum = CS_randInt(0, 3);
	} 

	//hit floor, bounce back up
	if (self->collision & 8) {
		self->yVel = -self->alt_yVel / 2;
		self->xVel /= 2;
		if (self->directive) {
			self->directive--;
		}
	}
	//hit wall, bounce away
	if (self->collision & 5) {
		self->xVel = -self->xVel * 3 / 4;
	}

	//spin
	self->frameTimer += self->directive;
	if (self->frameTimer > 8) {
		self->frameTimer = 0;
		if (++self->frameNum > 4) {
			self->frameNum = 0;
		}
	}
	//timeout
	if (++self->scriptTimer > 200) {
		self->inUse = 0;
	}

	self->yVel += 0x38;
	if (self->yVel > 1500) {
		self->yVel = 1500;
	}
	self->xPos += self->xVel;
	self->yPos += self->yVel;
	self->alt_yVel = self->yVel;

	//frames
	self->frameRect.left = 208;
	self->frameRect.top = 32;
	if (self->frameNum & 1) {
		self->frameRect.left += 8;
	}
	if (self->frameNum & 2) {
		self->frameRect.top += 8;
	}
	if (self->scriptTimer > 160 && (self->scriptTimer & 2)) {
		self->frameRect.right = self->frameRect.left;
	} else {
		self->frameRect.right = self->frameRect.left + 8;
		self->frameRect.bottom = self->frameRect.top + 8;
	}
}

void NPC_digSpot(CS_ENTITY* self) {
	if (!self->scriptState) {
		if (playerSpecialState == SSTATE_DIG_TRIGGER) {
			if (isHitPlayer(self)) {
				self->scriptState = 1;
				self->scriptTimer = 20;
				CS_runEvent(self->eventNum);
			}
		}
	} else {
		self->scriptTimer--;
		if (!self->scriptTimer) {
			self->scriptState = 0;
		}
	}
}

//default: U/D trigger (extends infinitely horizontal)
//alt direction: L/R trigger (extends infinitely vertical)
//state 1: player is left/above trigger
//state 2: player is right/under trigger
void NPC_boundsTrigger(CS_ENTITY* self) {
	int location;
	int target;

	// init the NPC to set default triggered state to be 
	// whichever side the player is already on
	if (self->scriptState == 0) {
		if (self->direction) {
			self->scriptState = (*CS_playerX < self->xPos) ? 1 : 2;
		} else {
			self->scriptState = (*CS_playerY < self->yPos) ? 1 : 2;
		}
	}
	//set bit 0
	int state = (self->scriptState - 1);
	//set bit 1
	state |= (self->direction == 0) ? 0 : 2;

	switch (state) {
	default:
	case 0:
		// U/D trigger, player above
		location = *CS_playerY;
		target = self->yPos + self->hitRect.bottom;
		break;
	case 1:
		// U/D trigger, player below
		location = self->yPos - self->hitRect.top;
		target = *CS_playerY;
		break;
	case 2:
		// L/R trigger, player left
		location = *CS_playerX;
		target = self->xPos + self->hitRect.right;
		break;
	case 3:
		// L/R trigger, player right;
		location = self->xPos - self->hitRect.left;
		target = *CS_playerX;
		break;
	}

	if (location > target) {
		//run event
		CS_runEvent(self->eventNum + self->scriptState - 1);
		self->scriptState = self->scriptState == 1 ? 2 : 1;
	}
}
