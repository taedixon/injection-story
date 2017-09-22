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
		case SSTATE_SWING_ENDLAG1:
		case SSTATE_SWING_ENDLAG2:
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
	unsigned char sfx[] = { 0x2f, 0x6E };
	if (*CS_playerDirection) {
		// facing right
		xOff = 0x2000;
	} else {
		xOff = -0x2000;
	}
	int bulletID = 1;
	if (CS_checkItem(20)) {
		bulletID++;
	}
	CS_createBullet(bulletID, *CS_playerX + xOff, *CS_playerY + yOff, *CS_playerDirection);
	CS_playSound(sfx[CS_randInt(0, 1)], 1);
}

//ID 1 (replaces snake)
void bullet_shovel(CS_BULLET* self) {
	self->framesActive++;
	if (self->framesActive > self->maxFrame) {
		self->inUse = 0;
	}
}

void(*bulletFuncs[])(CS_BULLET*) = {
	//NULL, // zero; null bullet
	&bullet_shovel, &bullet_shovel, &bullet_shovel, //1-3 - shovel
	0x4047B0, 0x4047B0, 0x4047B0, //4-6 - polar star
	NULL, NULL, NULL, //7-9 - fireball
	NULL, NULL, NULL, //10-12 - machine gun
	NULL, NULL, NULL, // 13-15 - missile launcher
	NULL, NULL, NULL, // 16-18 launcher explosions
	NULL, NULL, NULL, // 19-21 bubbler
	NULL, NULL, NULL, // 22-24 bubbler spurs, blade slashes, other
	NULL, NULL, NULL, // 25-27 blade
	NULL, NULL, NULL, // 28-30 SML
	NULL, NULL, NULL, // 31-33 SML explosions
	0x408230, 0x408230, 0x408230, // 34-36 Nemesis
	0x408710, 0x408710, 0x408710, // 37-39 spur shots
	0x408AE0, 0x408AE0, 0x408AE0, // 40-42 spur trails
	0x408230, NULL, NULL, // 43 curly's nemesis 44 ClearEnemy 45 whimsical star
};


void drawBullet_debug(CS_BULLET* self, int camX, int camY) {

	if (1) {
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
}

void drawBullet(int camX, int camY) {
	int i;
	int bulletX, bulletY;
	CS_BULLET* bullet;
	for (i = 0; i < 64; i++) {
		bullet = &CS_bullets[i];
		if (bullet->inUse) {
			bulletX = bullet->xPos;
			bulletY = bullet->yPos;
			switch (bullet->direction) {
			case 0:
				bulletX -= bullet->sizeRect.left;
				bulletY -= bullet->sizeRect.top;
				break;
			case 2:
				bulletX -= bullet->sizeRect.right;
				bulletY -= bullet->sizeRect.top;
				break;
			case 1:
				bulletX -= bullet->sizeRect.top;
				bulletY -= bullet->sizeRect.left;
				break;
			case 3:
				bulletX -= bullet->sizeRect.top;
				bulletY -= bullet->sizeRect.right;
			}
			toScreenSpace(&bulletX, &bulletY, camX, camY);
			CS_putBitmap3(CS_fullScreenRect, bulletX, bulletY, &bullet->frameRect, CS_BM_BULLET);
			
			//drawBullet_debug(bullet, camX, camY);
		}
	}

}
