#include "stdafx.h"
#include "npc.h"

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

	if (playerHitbox.right < npcHitbox.left ||
		playerHitbox.bottom < npcHitbox.top ||
		playerHitbox.left > npcHitbox.right ||
		playerHitbox.top > npcHitbox.bottom) {
		return 0;
	} else {
		// eliminating all possibilities for the hitboxes to be disjoint,
		// the only possibility that remains is that they overlap.
		return 1;
	}
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
