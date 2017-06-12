#include "stdafx.h"
#include <cavestory.h>
#include "util.h"


void control_topdown(char canControl) {
	int held = *CS_keyHeld;
	int pressed = *CS_keyPressed;
	int speed = 40;
	int speedcap = 0x300;
	int friction = 10;
	int frictionX, frictionY;

	
	if (held & *CS_keyLeft) {
		*CS_playerXvel += -speed;
	} 
	if (held & *CS_keyUp) {
		*CS_playerYvel += -speed;
	} 
	if (held & *CS_keyRight) {
		*CS_playerXvel += speed;
	} 
	if (held & *CS_keyDown) {
		*CS_playerYvel += speed;
	}

	vecFromAngle(&frictionX, &frictionY, friction, atan2(*CS_playerYvel, *CS_playerXvel));
	*CS_playerXvel -= frictionX;
	*CS_playerYvel -= frictionY;
	capMagnitude(CS_playerXvel, CS_playerYvel, speedcap);

	if (abs(*CS_playerXvel) < friction) {
		*CS_playerXvel = 0;
	}
	if (abs(*CS_playerYvel) < friction) {
		*CS_playerYvel = 0;
	}

	//update position and camera
	*CS_playerX += *CS_playerXvel;
	*CS_playerY += *CS_playerYvel;
	*CS_playerCamX = *CS_playerX;
	*CS_playerCamY = *CS_playerY;
}

void _drawPlayer_legacy(int camX, int camY) {
	//NOTE does not draw air tank bubble
	int wpnYOffset, wpnXOffset;
	RECT playerRect = *CS_playerFrameRect;
	RECT weaponRect;
	int playerScreenX = (*CS_playerX - (*CS_playerSizeRect).left) / 512 - camX / 512;
	int playerScreenY = (*CS_playerY - (*CS_playerSizeRect).top) / 512 - camY / 512;

	//calculate weapon framerects
	weaponRect.left = 24 * (CS_playerWeapons[*CS_playerSelWeapon].id % 13);
	weaponRect.right = weaponRect.left + 24;
	weaponRect.top = 96 * (CS_playerWeapons[*CS_playerSelWeapon].id / 13);
	weaponRect.bottom = weaponRect.top + 16;
	if (*CS_playerDirection == 2) {
		weaponRect.top += 16;
		weaponRect.bottom += 16;
	}
	if (*CS_playerFacingUp) {
		wpnYOffset = -4;
		weaponRect.top += 32;
		weaponRect.bottom += 32;
	} else if (*CS_playerFacingDown) {
		wpnYOffset = 4;
		weaponRect.top += 64;
		weaponRect.bottom += 64;
	} else {
		wpnYOffset = 0;
	}
	if (*CS_playerDirection) {
		wpnXOffset = 0;
	} else {
		wpnXOffset = -8;

	}
	if (*CS_playerFrameNum == 1 || *CS_playerFrameNum == 3 ||
			*CS_playerFrameNum == 6 || *CS_playerFrameNum == 8) {
		++weaponRect.top;
	}
	//draw the weapon
	CS_putBitmap3(CS_fullScreenRect, playerScreenX + wpnXOffset,
		playerScreenY + wpnYOffset, &weaponRect, CS_BM_ARMS);

	//draw the player
	if (*CS_playerEquipFlags & 0x40) {
		playerRect.top += 32;
		playerRect.bottom += 32;
	}
	CS_putBitmap3(CS_fullScreenRect, playerScreenX, playerScreenY, &playerRect, CS_BM_PLAYER);

}

void _drawPlayer_boat(int camX, int camY) {

}

void drawPlayer(int camX, int camY) {
	switch (*CS_controlMode) {
	case 0:
		_drawPlayer_legacy(camX, camY);
		break;
	case 1:
		_drawPlayer_boat(camX, camY);
		break;
	}
}

void _playerCalcFrame(int canControl) {
	CS_playerFrameRect->left = 0;
	CS_playerFrameRect->top = 0;
	CS_playerFrameRect->right = 16;
	CS_playerFrameRect->bottom = 16;
}

void _playerCalcFrame_boat(int canControl) {

}

void playerCalcFrame(char canControl) {
	switch (*CS_controlMode) {
	case 0:
		_playerCalcFrame(canControl);
		break;
	case 1:
		_playerCalcFrame(canControl);
		break;
	}
}
