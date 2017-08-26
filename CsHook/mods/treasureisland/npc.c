#include "stdafx.h"
#include "npc.h"
#include "util.h"

int _isHitPlayer(CS_ENTITY* ent) {
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
	int hit = _isHitPlayer(self);
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
}
