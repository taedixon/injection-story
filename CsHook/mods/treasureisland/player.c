#include "stdafx.h"
#include <cavestory.h>
#include "util.h"

int playerIdle = 0;
int playerWalkTimer;

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
	int playerScreenX = (*CS_playerX - (*CS_playerSizeRect).left) / CS_SUBPX - camX / CS_SUBPX;
	int playerScreenY = (*CS_playerY - (*CS_playerSizeRect).top) / CS_SUBPX - camY / CS_SUBPX;

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

void _drawPlayer_debug(int camX, int camY) {
	RECT fr;
	int hitboxCol = 0xFF2222;
	fr.left = -CS_playerHitRect->left - camX + *CS_playerX;
	fr.top = -CS_playerHitRect->top - camY + *CS_playerY;
	fr.right = CS_playerHitRect->right - camX + *CS_playerX;
	fr.bottom = CS_playerHitRect->bottom - camY + *CS_playerY;

	fr.left /= CS_SUBPX;
	fr.top /= CS_SUBPX;
	fr.right /= CS_SUBPX;
	fr.bottom /= CS_SUBPX;

	drawRect(&fr, hitboxCol);

	fr.left = (-camX + *CS_playerX) / CS_SUBPX - 1;
	fr.top = (-camY + *CS_playerY)  / CS_SUBPX - 1;
	fr.right = fr.left + 3;
	fr.bottom = fr.top + 3;
	drawRect(&fr, 0xFF00FF);
}

void drawPlayer(int camX, int camY) {
	switch (*CS_controlMode) {
	case 0:
		_drawPlayer_legacy(camX, camY);
		_drawPlayer_debug(camX, camY);
		break;
	case 1:
		//NOTE change if boat gets different draw rules
		_drawPlayer_legacy(camX, camY);
		break;
	}
}

void _playerCalcFrame(int canControl) {
	const int frameW = 48;
	const int frameH = 48;
	const int directionOffset = 240;
	const int nIdleFrame = 4;
	const int idleSpeed = 12;
	const int nWalkFrame = 6;
	const int walkSpeed = 8;

	int frameRow = 0, frameCol = 0;

	CS_playerSizeRect->left = frameW * CS_SUBPX / 2;
	CS_playerSizeRect->top = frameH * CS_SUBPX * 3 / 4;
	CS_playerHitRect->top = frameW * CS_SUBPX / 2;
	CS_playerHitRect->bottom = frameH * CS_SUBPX / 4;
	CS_playerHitRect->left = 4 * CS_SUBPX;
	CS_playerHitRect->right = 4 * CS_SUBPX;


	if ((*CS_playerTileFlags) & 0x38) {
		//player is on the ground
		if (*CS_playerXvel != 0) {
			// walking left/right
			playerIdle = 0;
			frameRow = 1;
			if (++playerWalkTimer >= nWalkFrame*walkSpeed) {
				playerWalkTimer = 0;
			}
			frameCol = playerWalkTimer / walkSpeed;
		} else {
			//idling on ground
			playerWalkTimer = 0;
			frameRow = 0;
			if (++playerIdle >= nIdleFrame*idleSpeed) {
				playerIdle = 0;
			}
			frameCol = playerIdle / idleSpeed;
		}
	} else {
		//in the air
		frameRow = 1;
		if (*CS_playerYvel < 0) {
			frameCol = 2;
		} else {
			frameCol = 3;
		}
	}

	CS_playerFrameRect->left = frameCol * frameW;
	CS_playerFrameRect->right = CS_playerFrameRect->left + frameW;
	CS_playerFrameRect->top = frameRow * frameH;
	CS_playerFrameRect->bottom = CS_playerFrameRect->top + frameH;

	//alt direction
	if (!(*CS_playerDirection)) {
		CS_playerFrameRect->top += directionOffset;
		CS_playerFrameRect->bottom += directionOffset;
	}
}

void _playerCalcFrame_boat(int canControl) {
	const int frameW = 16;
	const int frameH = 16;
	enum DIRECTION dir = getDirection(*CS_playerXvel, *CS_playerYvel);
	CS_playerFrameRect->left = dir * 0x10;
	CS_playerFrameRect->top = 0x20;
	CS_playerFrameRect->right = CS_playerFrameRect->left + 0x10;
	CS_playerFrameRect->bottom = CS_playerFrameRect->top + 0x10;
}

void playerCalcFrame(char canControl) {
	switch (*CS_controlMode) {
	case 0:
		_playerCalcFrame(canControl);
		break;
	case 1:
		_playerCalcFrame_boat(canControl);
		break;
	}
}


void playerCheckTiles(void)
{
	int tileType;
	int x, y; 
	int tileX;
	int tileY; 

	tileX = *CS_playerX / 16 / CS_SUBPX;
	tileY = *CS_playerY / 16 / CS_SUBPX;
	for (y = tileY - 1; y < tileY + 3; y++) {
		for (x = tileX - 1; x < tileX + 3; x++) {
			tileType = CS_GetTileType(x, y);
			switch (tileType) {
			case 5:
			case 0x41:
			case 0x43:
			case 0x46:
				*CS_playerTileFlags |= CS_playerHitSquareBlock(x, y);
				break;
			case 0x50:
				*CS_playerTileFlags |= CS_playerHitSlopeFloorA(x, y);
				break;
			case 0x51:
				*CS_playerTileFlags |= CS_playerHitSlopeFloorB(x, y);
				break;
			case 0x52:
				*CS_playerTileFlags |= CS_playerHitSlopeFloorC(x, y);
				break;
			case 0x53:
				*CS_playerTileFlags |= CS_playerHitSlopeFloorD(x, y);
				break;
			case 0x54:
				*CS_playerTileFlags |= CS_playerHitSlopeRoofA(x, y);
				break;
			case 0x55:
				*CS_playerTileFlags |= CS_playerHitSlopeRoofB(x, y);
				break;
			case 0x56:
				*CS_playerTileFlags |= CS_playerHitSlopeRoofC(x, y);
				break;
			case 0x57:
				*CS_playerTileFlags |= CS_playerHitSlopeRoofD(x, y);
				break;
			case 0x42:
				*CS_playerTileFlags |= CS_playerHitSpike(x, y);
				break;
			case 0x62:
				*CS_playerTileFlags |= CS_playerHitSpikeWater(x, y);
				break;
			case 0x80:
				*CS_playerTileFlags |= CS_playerHitWindLeft(x, y);
				break;
			case 0x81:
				*CS_playerTileFlags |= CS_playerHitWindUp(x, y);
				break;
			case 0x82:
				*CS_playerTileFlags |= CS_playerHitWindRight(x, y);
				break;
			case 0x83:
				*CS_playerTileFlags |= CS_playerHitWindDown(x, y);
				break;
			case 2:
				*CS_playerTileFlags |= CS_playerHitWater(x, y);
				break;
			case 0x60:
				*CS_playerTileFlags |= CS_playerHitWater(x, y);
				break;
			case 0x61:
				*CS_playerTileFlags |= CS_playerHitSquareBlock(x, y);
				*CS_playerTileFlags |= CS_playerHitWater(x, y);
				break;
			case 0x70:
				*CS_playerTileFlags |= CS_playerHitSlopeFloorA(x, y);
				*CS_playerTileFlags |= CS_playerHitWater(x, y);
				break;
			case 0x71:
				*CS_playerTileFlags |= CS_playerHitSlopeFloorB(x, y);
				*CS_playerTileFlags |= CS_playerHitWater(x, y);
				break;
			case 0x72:
				*CS_playerTileFlags |= CS_playerHitSlopeFloorC(x, y);
				*CS_playerTileFlags |= CS_playerHitWater(x, y);
				break;
			case 0x73:
				*CS_playerTileFlags |= CS_playerHitSlopeFloorD(x, y);
				*CS_playerTileFlags |= CS_playerHitWater(x, y);
				break;
			case 0x74:
				*CS_playerTileFlags |= CS_playerHitSlopeRoofA(x, y);
				*CS_playerTileFlags |= CS_playerHitWater(x, y);
				break;
			case 0x75:
				*CS_playerTileFlags |= CS_playerHitSlopeRoofB(x, y);
				*CS_playerTileFlags |= CS_playerHitWater(x, y);
				break;
			case 0x76:
				*CS_playerTileFlags |= CS_playerHitSlopeRoofC(x, y);
				*CS_playerTileFlags |= CS_playerHitWater(x, y);
				break;
			case 0x77:
				*CS_playerTileFlags |= CS_playerHitSlopeRoofD(x, y);
				*CS_playerTileFlags |= CS_playerHitWater(x, y);
				break;
			case 0xA0:
				*CS_playerTileFlags |= CS_playerHitWindLeft(x, y);
				*CS_playerTileFlags |= CS_playerHitWater(x, y);
				break;
			case 0xA1:
				*CS_playerTileFlags |= CS_playerHitWindUp(x, y);
				*CS_playerTileFlags |= CS_playerHitWater(x, y);
				break;
			case 0xA2:
				*CS_playerTileFlags |= CS_playerHitWindRight(x, y);
				*CS_playerTileFlags |= CS_playerHitWater(x, y);
				break;
			case 0xA3:
				*CS_playerTileFlags |= CS_playerHitWindDown(x, y);
				*CS_playerTileFlags |= CS_playerHitWater(x, y);
				break;
			default:
				break;
			}
		}
	}
	if (*CS_playerY > (*CS_globalWaterDepth) + 2048)
		*CS_playerTileFlags |= 0x100u;
}
