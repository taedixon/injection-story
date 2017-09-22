#include "stdafx.h"
#include "boss.h"
#include "util.h"
#include <math.h>

int hasReturnedServe = 0;

void findRay(int x1, int y1, int x2, int y2, int magnitude, int* xComponent, int* yComponent) {
	double atan = atan2(x2 - x1, y2 - y1);
	*xComponent = (int)(magnitude * sin(atan));
	*yComponent = (int)(magnitude * cos(atan));
}

#define STATE_DEFAULT 0
#define STATE_WAIT 1
#define STATE_LOWER 5
#define STATE_LOWER_2 6
#define STATE_IDLE 20
#define STATE_IDLE_2 21
#define STATE_CHOOSE_ATTACK 30
#define STATE_CHOOSE_ATTACK_2 31
#define STATE_DASH 32
#define STATE_DASH_2 33
#define STATE_ROLL_ATTACK 34
#define STATE_ROLL_2 35
#define STATE_TENNIS_ATTACK 36
#define STATE_TENNIS_ATTACK_SERVE 37
#define STATE_TENNIS_ATTACK_VOLLEY 38
#define STATE_TENNIS_ATTACK_CRASH 39
#define STATE_RECOIL 40
#define STATE_RECOIL_2 41
#define STATE_RECOIL_3 42
#define STATE_RECOIL_4 43
#define STATE_TENNIS_ATTACK_WAIT 44
#define STATE_DIVEBOMB 45
#define STATE_DIVEBOMB_MOVE 46
#define STATE_DIVEBOMB_WAIT 47
#define STATE_DIVEBOMB_FALL 48
#define STATE_DIVEBOMB_LAND 49
#define STATE_DIVEBOMB_WAIT_2 50
#define STATE_DIE 100
#define STATE_DIE_2 101
#define STATE_INVALID 999
void NPC_finalBoss(CS_ENTITY* self) {

	const int SPRITE_SZ = 64;
	const int ROW_SZ = 10;

	RECT nilRect = { 0, 0, 0, 0 };
	RECT* rect;
	int amount;
	int xVel, yVel;
	int i;

	if (self->scriptState < STATE_DIE && (self->health <= 0))
		self->scriptState = STATE_DIE;
	switch (self->scriptState) {
	case STATE_DEFAULT:
		//initialize
		CS_createNPC(288, 0, 0, 0, 0, 0, self, 0);
		self->scriptState = STATE_WAIT;
		self->frameTimer = 0;
		break;

	case STATE_WAIT:
		self->yVel = 0;
		self->xVel = 0;

		++self->frameTimer;
		if (self->frameTimer > 15) {
			self->frameTimer = 0;
			++self->frameNum;
		}
		if (self->frameNum > 3)
			self->frameNum = 0;
		break;
	case STATE_LOWER:
		self->targetY = self->yPos + 0x18000;
		self->scriptTimer = 0;
		self->scriptState = STATE_LOWER_2;
	case STATE_LOWER_2:
		amount = self->targetY - self->yPos;
		self->yVel = amount / 60;
		++self->frameTimer;
		if (self->frameTimer > 15) {
			self->frameTimer = 0;
			++self->frameNum;
		}
		if (self->frameNum > 3)
			self->frameNum = 0;

		if (++self->scriptTimer > 160) {
			self->scriptState = STATE_WAIT;
		}
		break;

	case STATE_IDLE:
		//pre-attack
		self->scriptState = STATE_IDLE_2;
		self->scriptTimer = 0;
		self->frameNum = 0;
		self->frameTimer = 0;
		self->damage = 0;
		self->flags |= 0x20u;
		self->flags &= -9;
	case STATE_IDLE_2:
		//idle between attacks
		self->xVel = 7 * self->xVel / 8;
		self->yVel = 7 * self->yVel / 8;
		++self->frameTimer;
		if (self->frameTimer > 12) {
			self->frameTimer = 0;
			++self->frameNum;
		}
		if (self->frameNum > 3)
			self->frameNum = 0;
		++self->scriptTimer;
		if (self->scriptTimer > 50)
			self->scriptState = STATE_CHOOSE_ATTACK;
		if (self->xPos >= *CS_playerX)
			self->direction = 0;
		else
			self->direction = 2;
		if (self->health < self->directive - 50) {
			self->directive = self->health;
			*((int*)0x4BBA30) = 10;
		}
			 break;
	case STATE_CHOOSE_ATTACK:
		//begin next attack
		self->scriptState = STATE_CHOOSE_ATTACK_2;
		self->scriptTimer = 0;
		self->frameNum = 4;
		self->xVel = 0;
		self->yVel = 0;
	case STATE_CHOOSE_ATTACK_2:
		//determine next move
		++self->scriptTimer;
		if (self->scriptTimer > 8) {
			self->scriptTimer = 0;
			//object timer => # of consecutive dashes
			if (CS_randInt(0, self->objectTimer) <= 0) {
				self->scriptState = STATE_DASH;
				self->objectTimer++;
			} else {
				self->objectTimer = 0;
				switch (CS_randInt(0, 8)) {
				case 1:
				case 0:
				case 2:
				case 3:
					self->scriptState = STATE_ROLL_ATTACK;
					break;
				case 4:
				case 5:
				case 6:
					self->scriptState = STATE_TENNIS_ATTACK;
					break;
				case 7:
				case 8:
					self->scriptState = STATE_DIVEBOMB;
					break;
				default:
					break;
				}
			}
		}
		break;
	case STATE_DASH:
		//calculate target [dash]
		self->scriptState = STATE_DASH_2;
		self->scriptTimer = 0;
		self->flags &= 0xFFDFu;
		amount = CS_randInt(20000, 60000);
		if (*CS_playerX >= self->xPos)
			self->targetX = *CS_playerX + amount;
		else
			self->targetX = *CS_playerX - amount;

		self->targetY = *CS_playerY - CS_randInt(20000, 60000);

		findRay(self->xPos, self->yPos, self->targetX, self->targetY, 0x800, &self->xVel, &self->yVel);
		self->flags |= 8;

		//shot a bullet
		if (self->objectTimer > 1) {
			findRay(self->xPos, self->yPos, *CS_playerX, *CS_playerY, 0x500, &xVel, &yVel);
			CS_createNPC(290, self->xPos, self->yPos, xVel, yVel, 0, 0, 0);
		}
		CS_playSound(113, 1);
		if (self->xVel <= 0)
			self->direction = 0;
		else
			self->direction = 2;
	case STATE_DASH_2:
		//move to target [dash]
		++self->scriptTimer;
		if (self->scriptTimer / 2 % 2)
			self->frameNum = 5;
		else
			self->frameNum = -1;
		int distToTarget = abs(self->xPos - self->targetX) + abs(self->yPos - self->targetY);
		if (distToTarget < 0x3000 || self->collision || self->scriptTimer > 300) {
			self->scriptState = STATE_IDLE;
		}
		break;
	case STATE_ROLL_ATTACK:
		//calculate target [attack]
		self->scriptState = STATE_ROLL_2;
		self->scriptTimer = 0;
		self->damage = 2;
		self->targetX = *CS_playerX;
		self->targetY = *CS_playerY;
		findRay(self->xPos, self->yPos, self->targetX, self->targetY, 0x500, &self->xVel, &self->yVel);

		self->flags &= 0xFFF7u;
		if (self->xVel <= 0)
			self->direction = 0;
		else
			self->direction = 2;
	case STATE_ROLL_2:
		//move [attack]
		++self->scriptTimer;
		distToTarget = abs(self->xPos - self->targetX) + abs(self->yPos - self->targetY);
		if (self->damageNumberTimer) {
			self->scriptState = STATE_RECOIL;
		} else if (distToTarget < 0x2000 || self->collision & 5) {
			self->scriptState = STATE_IDLE;
		}
		++self->frameTimer;
		if (self->frameTimer > 1) {
			self->frameTimer = 0;
			++self->frameNum;
		}
		if (self->frameNum > 9)
			self->frameNum = 6;
		if (self->scriptTimer % 5 == 1)
			CS_playSound(109, 1);
		break;

	case STATE_TENNIS_ATTACK:
		self->scriptTimer = 0;
		self->scriptState = STATE_TENNIS_ATTACK_SERVE;
		self->frameTimer = 0;
		CS_playSound(103, 1);

	case STATE_TENNIS_ATTACK_SERVE:
		if (++self->scriptTimer > 50) {
			self->scriptState = STATE_TENNIS_ATTACK_VOLLEY;
			self->frameTimer = 0;
			self->frameNum = 13;
			self->scriptTimer = 0;
			findRay(self->xPos, self->yPos, *CS_playerX, *CS_playerY, 0x500, &xVel, &yVel);
			CS_createNPC(285, self->xPos, self->yPos, xVel, yVel, 0, self, 0x180);
			CS_playSound(49, 1);
		}

		++self->frameTimer;
		self->frameNum = !(self->frameTimer % 8) ? 14 : 15;
		break;

	case STATE_TENNIS_ATTACK_WAIT:
		self->frameTimer++;
		if (self->frameTimer > 8) {
			self->frameTimer = 0;
			if (++self->frameNum > 3) {
				self->frameNum = 0;
			}
		}
		break;

	case STATE_TENNIS_ATTACK_VOLLEY:
		self->frameNum = 13;
		if (++self->scriptTimer > 8) {
			self->scriptState = STATE_TENNIS_ATTACK_WAIT;
			self->scriptTimer = 0;
		}
		break;

	case STATE_DIVEBOMB:
		self->scriptState = STATE_DIVEBOMB_MOVE;
		self->scriptTimer = 0;
		self->flags &= 0xFFDFu;

		self->targetX = *CS_playerX;
		self->targetY = (*CS_mapHeight << 13) / 3;
		
		findRay(self->xPos, self->yPos, self->targetX, self->targetY, 0x800, &self->xVel, &self->yVel);
		self->flags |= 8;
		CS_playSound(113, 1);

		if (self->xVel <= 0)
			self->direction = 0;
		else
			self->direction = 2;
	case STATE_DIVEBOMB_MOVE:
		//move to target [dash]
		++self->scriptTimer;
		if (self->scriptTimer / 2 % 2)
			self->frameNum = 5;
		else
			self->frameNum = -1;
		distToTarget = abs(self->xPos - self->targetX) + abs(self->yPos - self->targetY);
		if (distToTarget < 0x2000 || self->collision & 5) {
			self->scriptState = STATE_DIVEBOMB_WAIT;
			self->scriptTimer = 0;
			CS_playSound(8, 1);
			self->frameTimer = 0;
		}		
		break;
	case STATE_DIVEBOMB_WAIT:
		self->xVel = 7 * self->xVel / 8;
		self->yVel = 7 * self->yVel / 8;
		if (++self->scriptTimer > 10) {
			self->scriptState = STATE_DIVEBOMB_WAIT_2;
			self->scriptTimer = 0;
		}
		self->frameNum = !(self->scriptTimer & 8) ? 0 : 1;
		break;

	case STATE_DIVEBOMB_WAIT_2:
		self->xVel = 0;
		self->yVel = -0xC0;
		self->frameNum = (++self->frameTimer & 4) ? 16 : 17;
		if (++self->scriptTimer > 65) {
			self->yVel = 0x800;
			CS_playSound(101, 1);
			self->scriptState = STATE_DIVEBOMB_FALL;
			self->flags &= -9;
			self->hitRect.left *= 2;
			self->hitRect.right *= 2;
		}
		break;

	case STATE_DIVEBOMB_FALL:
		self->frameNum = 18;
		self->damage = 4;
		self->flags |= 0x20; 
		if (self->collision) {
			self->yVel = 0;
			self->hitRect.left /= 2;
			self->hitRect.right /= 2;
			self->scriptState = STATE_DIVEBOMB_LAND;
			self->scriptTimer = 0;
			CS_playSound(71, 1);
			for (i = 0; i < 4; i++) {
				CS_createNPC(4, self->xPos, self->yPos, 0, 0, 0, 0, 0x100);
			}

			CS_createNPC(289, self->xPos - 0x2000, self->yPos, -0x4FF, 0, 0, 0, 0);
			CS_createNPC(289, self->xPos + 0x2000, self->yPos, 0x4FF, 0, 2, 0, 0);

			if (self->health < 50) {
				//create more projectiles when low health
				CS_createNPC(290, self->xPos - 0x1000, self->yPos, -0x3FF, -0x32F, 0, 0, 0);
				CS_createNPC(290, self->xPos + 0x1000, self->yPos, 0x3FF, -0x32F, 0, 0, 0);
			}
		}
		break;
	case STATE_DIVEBOMB_LAND:
		self->damage = 0;
		self->frameNum = 11;
		if (++self->scriptTimer > 70) {
			self->scriptState = STATE_IDLE;
		}
		break;

	case STATE_RECOIL:
		//recoil
		self->scriptState = STATE_RECOIL_2;
		self->scriptTimer = 0;
		self->frameNum = 4;
		self->damage = 0;
		self->flags &= 0xFFF7u;
	case STATE_RECOIL_2:
		//recoil (flinch)
		self->xVel = 7 * self->xVel / 8;
		self->yVel = 7 * self->yVel / 8;
		++self->scriptTimer;
		if (self->scriptTimer > 6) {
			self->scriptState = STATE_RECOIL_3;
			self->scriptTimer = 0;
			self->yVel = -512;
			if (self->direction)
				self->xVel = -512;
			else
				self->xVel = 512;
		}
		break;
	case STATE_RECOIL_3:
		//recoil (fly back in air)
		self->frameNum = 10;
		if (self->collision & 8) {
			self->scriptState = STATE_RECOIL_4;
			self->scriptTimer = 0;
			self->frameNum = 4;
			if (self->xPos >= *CS_playerX)
				self->direction = 0;
			else
				self->direction = 2;
		}
		self->yVel += 32;
		if (self->yVel > 1535)
			self->yVel = 1535;
		break;
	case STATE_RECOIL_4:
		//recoil (recover)
		++self->scriptTimer;
		if (self->scriptTimer > 16)
			self->scriptState = STATE_IDLE;
		break;
	case 99:
		self->xVel = 0;
		self->yVel = 0;
		self->frameNum = 10;
		self->flags &= 0xFFDFu;
		break;
	case STATE_DIE:
		//shock
		CS_playSound(53, 1);
		self->scriptState = STATE_DIE_2;
		self->frameNum = 10;
		self->damage = 0;
		self->flags &= 0xFFDFu;
		self->flags &= -9;
		self->yVel = -512;
		self->damageNumberTimer += 50;
		*((int*)0x4BBAC0) += 1;
	case STATE_DIE_2:
		self->yVel += 32;
		if (self->collision) {
			self->scriptState = STATE_INVALID;
			self->frameNum = 11;
			self->xVel = 0;
			self->yVel = 0;
			CS_playSound(70, 1);
		}
		break;
	default:
		break;
	}
	self->yPos += self->yVel;
	if (self->damageNumberTimer)
		self->xPos += self->xVel / 2;
	else
		self->xPos += self->xVel;

	rect = &self->frameRect;
	if (self->frameNum < 0) {
		*rect = nilRect;
	} else {
		rect->left = (self->frameNum % ROW_SZ) * SPRITE_SZ;
		rect->top = (self->frameNum / ROW_SZ) * SPRITE_SZ;
		if (self->direction) {
			rect->top += SPRITE_SZ * 2;
		}
		rect->right = rect->left + SPRITE_SZ;
		rect->bottom = rect->top + SPRITE_SZ;
	}
}

void NPC_tennisBall(CS_ENTITY* self) {

	const int returnOdds[] = {
		100, 90, 50, 20, 0
	};

	if (self->scriptState == 0) {
		self->scriptState = 1;
		if (!hasReturnedServe) {
			CS_createNPC(286, 0, 0, 0, 0, 0, self, 0);
		}
	}

	if (self->collision) {
		self->parent->scriptState = STATE_IDLE;
		self->inUse = 0;
		CS_killNpc(self, 1);
	}
	if (self->scriptTimer < 10) {
		self->scriptTimer++;
	} else {
		int distToTarget = abs(self->xPos - self->parent->xPos) + abs(self->yPos - self->parent->yPos);
		if (distToTarget < 0x2000) {
			//check if we can return the serve
			if (CS_randInt(1, 100) > returnOdds[self->objectTimer]) {
				//we failed to return
				self->parent->scriptState = STATE_RECOIL;
				CS_killNpc(self, 1);
				//todo: create small bullet here instead
				self->parent->flags |= 0x20;
				CS_createBullet(0x18, self->parent->xPos, self->parent->yPos, 0);
			} else {
				//we successfully returned
				self->parent->scriptState = STATE_TENNIS_ATTACK_VOLLEY;
				self->xVel = (int)(-self->xVel * 1.08);
				self->yVel = (int)(-self->yVel * 1.08);
				self->scriptTimer = 0;
				self->objectTimer++;
				CS_playSound(111, 1);
			}
		}
	}

	if (self->damageNumberTimer) {
		//player hit ball
		self->xVel = (int)(-self->xVel * 1.08);
		self->yVel = (int)(-self->yVel * 1.08);
		self->damageNumberTimer = 0;
		CS_playSound(112, 1);
		hasReturnedServe = 1;
	}

	self->frameTimer++;

	self->xPos += self->xVel;
	self->yPos += self->yVel;

	self->frameRect.left = 0;
	self->frameRect.top = 256;
	if (self->frameTimer & 4) {
		self->frameRect.left += 16;
	}
	self->frameRect.right = self->frameRect.left + 16;
	self->frameRect.bottom = self->frameRect.top + 16;
}

void NPC_pleaseHitThis(CS_ENTITY* self) {
	RECT frames[] = {
			{ 0, 0, 0, 0 },
			{ 0, 272, 16, 288 },
	};

	self->inUse = self->parent->inUse;
	self->xPos = self->parent->xPos;
	self->yPos = self->parent->yPos - 0x2000;
	if (self->parent->health < 200) {
		//player has hit the bogey
		self->inUse = 0;
	}
	self->frameTimer++;
	if (self->frameTimer >= 16) {
		self->frameTimer = 0;
	}

	self->frameRect = self->frameTimer > 12 ? frames[0] : frames[1];
}

void NPC_splitFocus(CS_ENTITY* self) {
	int xDif, yDif;

	self->eventNum = self->parent->eventNum + 1;
	xDif = (self->parent->xPos - *CS_playerX)/2;
	yDif = (self->parent->yPos - *CS_playerY)/2;
	capMagnitude(&xDif, &yDif, 0x8000);
	self->xPos = *CS_playerX + xDif;
	self->yPos = *CS_playerY + yDif;
}

void NPC_shockwave(CS_ENTITY* self) {
	if (++self->scriptTimer > 60 || self->collision) {
		CS_killNpc(self, 1);
	}

	self->xPos += self->xVel;
	self->yPos += self->yVel;

	self->frameRect.top = 256;
	self->frameRect.bottom = 288;
	self->frameRect.left = 48;
	if (!self->direction) {
		self->frameRect.left += 16;
	}
	self->frameRect.right = self->frameRect.left + 16;
}

void NPC_simpleProjectile(CS_ENTITY* self) {
	RECT frame = { 32, 256, 40, 264 };

	if (self->collision) {
		CS_killNpc(self, 1);
	}

	self->xPos += self->xVel;
	self->yPos += self->yVel;

	if (++self->frameTimer & 4) {
		frame.left += 8;
		frame.right += 8;
	}

	self->frameRect = frame;
}
