#include "stdafx.h"
#include "npc.h"
#include "dd7.h"
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
