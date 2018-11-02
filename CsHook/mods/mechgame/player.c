#include "stdafx.h"
#include <cavestory.h>
#include "util.h"
#include "player.h"
#include "weapon.h"
#include "map.h"
#include "mech.h"

// custom player control vars
int playerIdle = 0;
int playerAnimState = 0;
int playerAnimTimer = 0;
int playerSpecialState = 0;

int prevSpecialState = 0;
int PrevTileFlags = 0;

int treasure = 0;
int treasureTimer = 0;
int treasureNumSpawnPoint;

void setPlayerSpecialState(int newState) {
	if (playerSpecialState != 0) {
		prevSpecialState = playerSpecialState;
	}
	playerSpecialState = newState;
	playerAnimState = 0;
	playerAnimTimer = 0;
	playerIdle = 0;
}

void control_topdown(char canControl) {
	int held = *CS_keyHeld;
	int pressed = *CS_keyPressed;
	int speed = 40;
	int speedcap = 0x300;
	int friction = 10;
	int frictionX, frictionY;
	*CS_playerStateFlags &= -2;

	if (canControl) {
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
		if (pressed & (*CS_keyShoot | *CS_keyJump)) {
			*CS_playerStateFlags |= 1;
		}
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

// called immediately after player agility code is run to allow us to
// do some stuff without needing to rewrite it entirely.
// gives us a chance to set special state in some conditions to 
// transition to a different control mode.
void _control_regular_postop(int canControl) {
	if (canControl) {
		if ((*CS_keyPressed & *CS_keyMap) && (*CS_playerTileFlags & 0x8)) {
			//press the dig key while on ground
			if (CS_checkFlag(99)) {
				//flag 99 enables digging
				setPlayerSpecialState(SSTATE_DIG);
			}
		}
	}
}

// if playerSpecialState is set, this gets called instead of regular player
// controls for special behaviours
void _control_override() {
	switch (playerSpecialState) {
	case SSTATE_SWING:
		//fallthru
	case SSTATE_SWING1:
	case SSTATE_SWING2:
	case SSTATE_SWING_ENDLAG1:
	case SSTATE_SWING_ENDLAG2:
		//same behaviour for digging
	case SSTATE_DIG:
	case SSTATE_DIG_TRIGGER:
		*CS_playerXvel -= *CS_playerXvel / 10;
		//move down to ensure we maintain tile contact
		*CS_playerYvel = 0x201;
		//exit special state if we fall off the floor
		if (!(*CS_playerTileFlags & 0x38)) {
			playerSpecialState = SSTATE_NONE;
		}
		break;

	}

	*CS_playerX += *CS_playerXvel;
	*CS_playerY += *CS_playerYvel;
}

void _drawPlayer_legacy(int camX, int camY) {

	//don't draw on invuln blink frames
	//don't draw if the player is D E A D
	if (*CS_playerInvulnTimer & 4
		|| *CS_playerCurrentHealth <= 0) {
		return;
	}

	//NOTE does not draw air tank bubble
	int wpnYOffset, wpnXOffset;
	RECT playerRect = *CS_playerFrameRect;
	RECT weaponRect;
	int playerScreenX = (*CS_playerX - CS_playerSizeRect->left);
	int playerScreenY = (*CS_playerY - CS_playerSizeRect->top);
	toScreenSpace(&playerScreenX, &playerScreenY, camX, camY);

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
	//CS_putBitmap3(CS_fullScreenRect, playerScreenX + wpnXOffset,
	//	playerScreenY + wpnYOffset, &weaponRect, CS_BM_ARMS);

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
	fr.left = -CS_playerHitRect->left + *CS_playerX;
	fr.top = -CS_playerHitRect->top + *CS_playerY;
	fr.right = CS_playerHitRect->right + *CS_playerX;
	fr.bottom = CS_playerHitRect->bottom + *CS_playerY;

	toScreenSpace(&fr.left, &fr.top, camX, camY);
	toScreenSpace(&fr.right, &fr.bottom, camX, camY);

	drawRect(&fr, hitboxCol);

	fr.left = *CS_playerX;
	fr.top = *CS_playerY;
	toScreenSpace(&fr.left, &fr.top, camX, camY);
	fr.left--;
	fr.right--;
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
		_drawPlayer_debug(camX, camY);
		break;
	}
}

void _calcFrame_ground(int* row, int* col) {
	const int nWalkFrame = 6;
	const int walkSpeed = 8;
	const int nIdleFrame = 4;
	const int idleSpeed = 12;
	//player is on the ground
	if (*CS_playerXvel != 0) {
		// walking left/right
		playerIdle = 0;
		*row = 1;
		if (++playerAnimTimer >= nWalkFrame*walkSpeed) {
			playerAnimTimer = 0;
		}
		*col = playerAnimTimer / walkSpeed;
	} else {
		//idling on ground
		playerAnimTimer = 0;
		*row = 0;
		if (++playerIdle >= nIdleFrame*idleSpeed) {
			playerIdle = 0;
		}
		*col = playerIdle / idleSpeed;
	}
}

void _calcFrame_air(int* row, int* col) {
	//in the air
	*row = 1;
	int yv = *CS_playerYvel;
	if (yv < -0x100) {
		*col = 6;
	} else if (yv < 0) {
		*col = 7;
	} else if (yv < 0x180) {
		*col = 8;
	} else {
		*col = 9;
	}
}

void _calcFrame_attack(int* row, int* col) {
	const int N_SWING_FRAME = 6;
	const int frameTiming[] = {
		8, 5, 4, 4, 7, 7
	};
	if (playerSpecialState == SSTATE_SWING) {
		if (prevSpecialState == SSTATE_SWING_ENDLAG1) {
			playerSpecialState = SSTATE_SWING2;
		} else {
			playerSpecialState = SSTATE_SWING1;
		}
	}

	if (playerSpecialState == SSTATE_SWING1 
		|| playerSpecialState == SSTATE_SWING_ENDLAG1) {
		*row = 2;
	} else {
		*row = 3;
	}
	*col = playerAnimState;
	if (++playerAnimTimer >= frameTiming[playerAnimState]) {
		playerAnimTimer = 0;
		++playerAnimState;
		if (playerAnimState == 3) {
			// on this frame we want to begin the hurtbox
			shovel_do_attack();
			playerSpecialState = (playerSpecialState == SSTATE_SWING1) ? SSTATE_SWING_ENDLAG1 : SSTATE_SWING_ENDLAG2;
		}
		if (playerAnimState >= N_SWING_FRAME) {

			setPlayerSpecialState(SSTATE_NONE);
		}
	}
}

void _calcFrame_dig(int* row, int* col) {
	const int N_DIG_FRAME = 4;
	const int frameTiming[] = {
		12, 8, 8, 12
	};
	playerSpecialState = SSTATE_DIG;
	*row = 0;
	*col = playerAnimState + 4;
	if (++playerAnimTimer >= frameTiming[playerAnimState]) {
		playerAnimTimer = 0;
		++playerAnimState;
		if (playerAnimState == 2) {
			// on this frame we want to begin the hurtbox
			CS_playSound(109, 1);
			playerSpecialState = SSTATE_DIG_TRIGGER;
		}
		if (playerAnimState >= N_DIG_FRAME) {

			setPlayerSpecialState(SSTATE_NONE);
		}
	}
}


void _playerCalcFrame(int canControl) {
	const int frameW = 64;
	const int frameH = 64;
	const int directionOffset = 64*4;

	int frameRow = 0, frameCol = 0;

	CS_playerSizeRect->left = frameW * CS_SUBPX / 2;
	CS_playerSizeRect->top = (frameH - 8) * CS_SUBPX;
	CS_playerHitRect->top = frameH * CS_SUBPX / 2;
	CS_playerHitRect->bottom = 8 * CS_SUBPX;
	CS_playerHitRect->left = 8 * CS_SUBPX;
	CS_playerHitRect->right = 8 * CS_SUBPX;


	frameRow = 0;
	frameCol = 0;

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
	CS_playerFrameRect->top = 192;
	CS_playerFrameRect->right = CS_playerFrameRect->left + 0x10;
	CS_playerFrameRect->bottom = CS_playerFrameRect->top + 0x10;
	//set boat hitbox
	CS_playerHitRect->top = frameH * CS_SUBPX / 4;
	CS_playerHitRect->bottom = frameH * CS_SUBPX / 3;
	CS_playerHitRect->left = 4 * CS_SUBPX;
	CS_playerHitRect->right = 4 * CS_SUBPX;
	CS_playerSizeRect->left = frameW * CS_SUBPX / 2;
	CS_playerSizeRect->top = frameH * CS_SUBPX / 2;
}

void playerCalcFrame(char canControl) {
	switch (*CS_controlMode) {
	case 0:
		calcFrameMech(canControl);
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
	for (y = tileY - 3; y < tileY + 3; y++) {
		for (x = tileX - 3; x < tileX + 3; x++) {
			tileType = CS_GetTileType(x, y);
			switch (tileType) {
			case 5:
			case 0x45:
				//custom tile: one-way floor
				*CS_playerTileFlags |= hitTile_oneway(x, y);
				break;
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

	// I think the game resets this before calling the tile checker
	// so, we'll set this value at the end of the function
	// rather than at the beginning like i'd prefer.
	PrevTileFlags = *CS_playerTileFlags;
}

void drawPlayerLife(char canControl) {
	int contextSegments = 8;
	int contextX, contextY;
	int i;
	RECT uiRect = {
		0, 144, 320, 144+26
	};
	//draw UI background
	CS_putBitmap3(CS_fullScreenRect, 0, 0, &uiRect, CS_BM_TEXTBOX);

	uiRect.left = 236;
	uiRect.right = 244;
	int healthRectY = 40;
	//draw the health blips
	for (i = 0; i < 12 && i < *CS_playerMaxHealth; i++) {
		contextX = 1 + (i % 4) * 8;
		contextY = 1 + (i / 4) * 8;
		uiRect.top = healthRectY;
		if (i >= *CS_playerCurrentHealth) {
			uiRect.top += 8;
		}
		uiRect.bottom = uiRect.top + 8;
		CS_putBitmap3(CS_fullScreenRect, contextX, contextY, &uiRect, CS_BM_TEXTBOX);
	}

}

void drawPlayerArms(char canControl) {
	drawMechUI();
}

void drawMapName(int mode) {
	if (*CS_isShowingMapName) {
		int tMax = 160;
		int scrollTime = 12;
		RECT clipRect = {
			88, 7, 232, 16
		};
		RECT* CS_mapNameRect = (RECT*)0x493650;
		int timer = *CS_mapNameTimer;
		int mnaX = 80;
		int mnaY = 7;
		if (timer < scrollTime) {
			mnaY += scrollTime - timer;
		} else if (timer >(tMax - scrollTime)) {
			mnaY -= timer - (tMax - scrollTime);
		}
		CS_putBitmap3(&clipRect, mnaX, mnaY, CS_mapNameRect, CS_BM_MNA);

		if (timer++ > tMax) {
			*CS_isShowingMapName = 0;
			timer = 0;
		}
		*CS_mapNameTimer = timer;
	}
}

void playerAct(int canControl) {
	treasureNumSpawnPoint = *CS_playerY - CS_playerHitRect->top;
	if (*CS_playerStateFlags & 0x80) {
		if (*CS_playerInvulnTimer > 0) {
			--*CS_playerInvulnTimer;
		}
		if (treasureTimer > 0) {
			--treasureTimer;
		} else if (*CS_expToGain) {
			CS_createNumberObject(CS_playerX, &treasureNumSpawnPoint, *CS_expToGain);
			*CS_expToGain = 0;
		}
		switch (*CS_controlMode) {
		case 0:
			controlMech(canControl);
			break;
		case 1:
			control_topdown(canControl);
			break;
		}
	}
	*CS_playerStateFlags &= -0x21;
}

void getCoin(int coinAmt) {
	int amtGained = CS_randInt(8*coinAmt, 12*coinAmt);

	*CS_expToGain += amtGained;
}

void resetTreasureCount() {
	treasure = 0;
}
