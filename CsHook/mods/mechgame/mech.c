#include "stdafx.h"
#include "mech.h"
#include <cavestory.h>

MechAttrib myMech = { 0 };

void initMech(MechAttrib* self) {

	self->init = 1;

	self->xPos = CS_playerX;
	self->yPos = CS_playerY;
	self->xVel = CS_playerXvel;
	self->yVel = CS_playerYvel;

	self->thrust = 0x30;
	self->speedCap = 0x380;
	self->friction = 0x20;
	self->jumpForce = 0x600;
	self->hoverForce = 0x78;
	self->gravity = 0x70;
	
	self->armourMax = 300;
	self->armourCurrent = 300;

	self->shieldMax = 100;
	self->shieldCurrent = 100;
	self->shieldRechargeRate = 1;

	self->batteryMax = 10000;
	self->batteryCurrent = 10000;
	self->batteryRechargeRate = 200;
	self->hoverCost = 300;

}

char drainBattery(MechAttrib* self, int amt) {
	if (self->batteryCurrent >= amt) {
		self->batteryCurrent -= amt;
		return 1;
	}
	return 0;
}

void passiveDrain(MechAttrib* self) {
	if (self->shieldCurrent < self->shieldMax) {
		if (drainBattery(self, self->shieldRechargeRate * 100)) {
			self->shieldCurrent += self->shieldRechargeRate;
			if (self->shieldCurrent > self->shieldMax) {
				self->shieldCurrent = self->shieldMax;
			}
		}
	}
}

void controlMechPreJump(MechAttrib* self, char canControl) {
	if (!self->scriptState) {
		self->scriptState++;
		self->scriptTimer1 = 10;
	} else {
		if (--self->scriptTimer1 <= 0) {
			self->scriptState = 0;
			self->controlState = NORMAL;
			*self->yVel -= self->jumpForce;
		}
	}
}

void controlMechCommon(MechAttrib* self) {

	if (*self->yVel < 0 && (*CS_keyHeld & *CS_keyJump) && !self->hoverOn) {
		*self->yVel += (int)(self->gravity * 0.6);
	} else {
		*self->yVel += self->gravity;
	}

	if (*self->xVel > self->speedCap) {
		*self->xVel = self->speedCap;
	} else if (*self->xVel < -self->speedCap) {
		*self->xVel = -self->speedCap;
	}

	*self->xPos += *self->xVel;
	*self->yPos += *self->yVel;
	*CS_playerCamX = *self->xPos;
	*CS_playerCamY = *self->yPos;

	self->batteryCurrent += self->batteryRechargeRate;
	if (self->batteryCurrent > self->batteryMax) {
		self->batteryCurrent = self->batteryMax;
	}

	if (*self->xVel < 0) {
		*CS_playerDirection = 0;
	} else if (*self->xVel > 0) {
		*CS_playerDirection = 1;
	}
}


void controlMechNormal(MechAttrib* self, char canControl) {
	if (!self->init) {
		initMech(self);
	}

	int kp = *CS_keyPressed;
	int kh = *CS_keyHeld;

	passiveDrain(self);

	// on the ground
	if (*CS_playerTileFlags & CS_TILEFLAG_HITFLOOR) {
		self->hoverOn = 0;
		int frictionAmt = min(self->friction, abs(*self->xVel));
		if (*self->xVel > 0) {
			*self->xVel -= frictionAmt;
		} else {
			*self->xVel += frictionAmt;
		}
		if (kp & *CS_keyJump) {
			self->scriptState = 0;
			self->controlState = PREJUMP;
			return controlMechPreJump(self, canControl);
		}
		if (kh & *CS_keyLeft) {
			*self->xVel -= self->thrust;
		}
		if (kh & *CS_keyRight) {
			*self->xVel += self->thrust;
		}
	} else {
		// in air
		if (!self->hoverOn) {
			if (kp & *CS_keyJump && drainBattery(self, 2000)) {
				self->hoverOn = 1;
				if (*self->yVel < 0) {
					*self->yVel *= 0.5;
				}
			}
		} else {
			if (kh & *CS_keyJump && drainBattery(self, self->hoverCost)) {
				*self->yVel -= self->hoverForce;
				if (*self->yVel > 0) {
					// double strength when moving down
					*self->yVel -= self->hoverForce;
				}
			} else {
				self->hoverOn = 0;
			}
		}

		if (kh & *CS_keyLeft) {
			*self->xVel -= self->thrust / 2;
		}
		if (kh & *CS_keyRight) {
			*self->xVel += self->thrust / 2;
		}
	}

}

void controlMech(char canControl) {
	MechAttrib* mech = &myMech;
	switch (mech->controlState) {
	case NORMAL:
		controlMechNormal(mech, canControl);
		break;
	case PREJUMP:
		controlMechPreJump(mech, canControl);
		break;
	default:
		break;
	}
	controlMechCommon(mech);
}


void drawMechUI() {
	MechAttrib* self = &myMech;
	RECT batteryRect = {
		228, 112, 244, 144
	};
	RECT batFrameRect = {
		212, 112, 228, 144
	};

	int batteryHeight = batteryRect.bottom - batteryRect.top;
	float pctBatteryRem = self->batteryCurrent / (float)self->batteryMax;
	int batteryLost = (int)(batteryHeight - batteryHeight * pctBatteryRem);
	batteryRect.top += batteryLost;

	CS_putBitmap3(CS_fullScreenRect, 320 - 16, batteryLost, &batteryRect, CS_BM_TEXTBOX);
	CS_putBitmap3(CS_fullScreenRect, 320 - 16, 0, &batFrameRect, CS_BM_TEXTBOX);
}

void calcFrameMech() {
	MechAttrib* self = &myMech;
	const int frameW = 64;
	const int frameH = 64;
	const int directionOffset = 64 * 4;

	int frameRow = 0, frameCol = 0;

	CS_playerSizeRect->left = frameW * CS_SUBPX / 2;
	CS_playerSizeRect->top = (frameH - 8) * CS_SUBPX;
	CS_playerHitRect->top = frameH * CS_SUBPX / 2;
	CS_playerHitRect->bottom = 8 * CS_SUBPX;
	CS_playerHitRect->left = 8 * CS_SUBPX;
	CS_playerHitRect->right = 8 * CS_SUBPX;


	switch (self->controlState) {
	case NORMAL:
		break;
	case PREJUMP:
		frameRow = 1;
		frameCol = 2 - (self->scriptTimer1 / 3);
		break;
	}

	if (frameCol <= 0) frameCol = 0;
	if (frameRow <= 0) frameRow = 0;

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
