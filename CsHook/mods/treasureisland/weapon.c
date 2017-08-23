#include "stdafx.h"

#include "weapon.h"
#include "player.h"
#include "util.h"

int inputBuffer;

void weapon_shovel(int lvl) {

	if (inputBuffer && playerSpecialState == SSTATE_NONE) {
		inputBuffer = 0;
		setPlayerSpecialState(SSTATE_SWING);
		return;
	}

	if (*CS_keyPressed & *CS_keyShoot 
		&& *CS_playerTileFlags & 0x38 ) {
		switch (playerSpecialState) {
		case SSTATE_NONE:
			setPlayerSpecialState(SSTATE_SWING);
			break;
		case SSTATE_SWING1:
		case SSTATE_SWING2:
			inputBuffer = 1;
			break;
		default:
			break;
		}
	}
}

void shovel_do_attack() {
	int xOff;
	int yOff = -0xA00;
	if (*CS_playerDirection) {
		// facing right
		xOff = 0x2000;
	} else {
		xOff = -0x2000;
	}
	CS_createBullet(1, *CS_playerX + xOff, *CS_playerY + yOff, *CS_playerDirection);
	CS_playSound(0x2E + CS_randInt(0, 1), 1);
}

//ID 1 (replaces snake)
void bullet_shovel(CS_BULLET* self) {
	self->framesActive++;
	if (self->framesActive > self->maxFrame) {
		self->inUse = 0;
		CS_createEffect(self->xPos, self->yPos, 0x0F, 0);
	}
}

void(*bulletFuncs[])(CS_BULLET*) = {
	&bullet_shovel, &bullet_shovel, &bullet_shovel
};

void drawBullet_debug(CS_BULLET* self, int camX, int camY) {
	RECT fr;
	int hitboxCol = 0xFF2222;
	fr.left = ((-self->hitRect.left + self->xPos) & -CS_SUBPX) - camX;
	fr.top = ((-self->hitRect.top + self->yPos) & -CS_SUBPX) - camY;
	fr.right = ((self->hitRect.left + self->xPos) & -CS_SUBPX) - camX;
	fr.bottom = ((self->hitRect.top + self->yPos) & -CS_SUBPX) - camY;

	fr.left /= CS_SUBPX;
	fr.top /= CS_SUBPX;
	fr.right /= CS_SUBPX;
	fr.bottom /= CS_SUBPX;

	drawRect(&fr, hitboxCol);

	fr.left = (-camX + self->xPos) / CS_SUBPX - 1;
	fr.top = (-camY + self->yPos) / CS_SUBPX - 1;
	fr.right = fr.left + 3;
	fr.bottom = fr.top + 3;
	drawRect(&fr, 0xFF00FF);
}
