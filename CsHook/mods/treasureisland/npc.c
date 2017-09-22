#include "stdafx.h"
#include "npc.h"
#include "util.h"
#include "player.h"

int isHitPlayer(CS_ENTITY* ent) {
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
	int hit = isHitPlayer(self);
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

	if (self->health < 1000) {
		//spawn 4x npc_potshard
		CS_killNpc(self, 1);
		int shards = CS_randInt(4, 6);
		for (int i = 0; i < shards; i++) {
			CS_createNPC(79, self->xPos, self->yPos, 0, 0, 0, 0, 0x100);
		}
	}
}

void NPC_potShard(CS_ENTITY* self) {
	if (!self->scriptState) {
		self->scriptState = 1;
		self->xVel = CS_randInt(-1000, 1000);
		self->yVel = CS_randInt(-1500, -2500);
		self->directive = CS_randInt(0, 4);
		self->frameNum = CS_randInt(0, 3);
	} 

	//hit floor, bounce back up
	if (self->collision & 8) {
		self->yVel = -self->alt_yVel / 2;
		self->xVel /= 2;
		if (self->directive) {
			self->directive--;
		}
	}
	//hit wall, bounce away
	if (self->collision & 5) {
		self->xVel = -self->xVel * 3 / 4;
	}

	//spin
	self->frameTimer += self->directive;
	if (self->frameTimer > 8) {
		self->frameTimer = 0;
		if (++self->frameNum > 4) {
			self->frameNum = 0;
		}
	}
	//timeout
	if (++self->scriptTimer > 200) {
		self->inUse = 0;
	}

	self->yVel += 0x38;
	if (self->yVel > 1500) {
		self->yVel = 1500;
	}
	self->xPos += self->xVel;
	self->yPos += self->yVel;
	self->alt_yVel = self->yVel;

	//frames
	self->frameRect.left = 208;
	self->frameRect.top = 32;
	if (self->frameNum & 1) {
		self->frameRect.left += 8;
	}
	if (self->frameNum & 2) {
		self->frameRect.top += 8;
	}
	if (self->scriptTimer > 160 && (self->scriptTimer & 2)) {
		self->frameRect.right = self->frameRect.left;
	} else {
		self->frameRect.right = self->frameRect.left + 8;
		self->frameRect.bottom = self->frameRect.top + 8;
	}
}

//alt direction: run event every time
//regular event: run event when health < 10
void NPC_digSpot(CS_ENTITY* self) {
	if (!self->scriptState) {
		if (playerSpecialState == SSTATE_DIG_TRIGGER && isHitPlayer(self)) {
			self->scriptState = 1;
			self->scriptTimer = 20;
			if (self->direction || self->health <= 10) {
				CS_runEvent(self->eventNum);
			} else {
				self->health -= 1;
			}
		}
	} else {
		self->scriptTimer--;
		if (!self->scriptTimer) {
			self->scriptState = 0;
		}
	}
}

//default: U/D trigger (extends infinitely horizontal)
//alt direction: L/R trigger (extends infinitely vertical)
//state 1: player is left/above trigger
//state 2: player is right/under trigger
void NPC_boundsTrigger(CS_ENTITY* self) {
	int location;
	int target;

	// init the NPC to set default triggered state to be 
	// whichever side the player is already on
	if (self->scriptState == 0) {
		if (self->direction) {
			self->scriptState = (*CS_playerX < self->xPos) ? 1 : 2;
		} else {
			self->scriptState = (*CS_playerY < self->yPos) ? 1 : 2;
		}
	}
	//set bit 0
	int state = (self->scriptState - 1);
	//set bit 1
	state |= (self->direction == 0) ? 0 : 2;

	switch (state) {
	default:
	case 0:
		// U/D trigger, player above
		location = *CS_playerY;
		target = self->yPos + self->hitRect.bottom;
		break;
	case 1:
		// U/D trigger, player below
		location = self->yPos - self->hitRect.top;
		target = *CS_playerY;
		break;
	case 2:
		// L/R trigger, player left
		location = *CS_playerX;
		target = self->xPos + self->hitRect.right;
		break;
	case 3:
		// L/R trigger, player right;
		location = self->xPos - self->hitRect.left;
		target = *CS_playerX;
		break;
	}

	if (location > target) {
		//run event
		CS_runEvent(self->eventNum + self->scriptState - 1);
		self->scriptState = self->scriptState == 1 ? 2 : 1;
	}
}

void NPC_dungeonEntrance(CS_ENTITY* self) {
	if (!self->scriptState) {
		self->scriptState = 1;
		self->targetX = self->xPos;
		self->targetY = self->yPos;
	}
	if (self->scriptState == 10) {
		self->targetY -= 0x2000 * self->direction;
		self->scriptState = 1;
		self->direction = 0;
	}

	char shake = 0;
	if (self->yPos > self->targetY) {
		self->yPos -= 0x80;
		shake = 1;
	} else if (self->yPos < self->targetY) {
		shake = 1;
		self->yPos += 0x80;
	}
	if (abs(self->yPos - self->targetY) < 0x80) {
		self->yPos = self->targetY;
	}

	self->xPos = self->targetX;
	if (shake) {
		self->xPos += CS_randInt(-0x400, 0x400);
	}

	self->frameRect.left = 32;
	self->frameRect.right = 80;
	self->frameRect.top = 144;
	self->frameRect.bottom = 208;
}

void NPC_beamBall(CS_ENTITY* self) {
	int SPRITE_W = 22;
	int SPRITE_H = 16;
	int frameX, frameY;
	int bwee;

	switch (self->scriptState) {
	case 0: //rest
		self->frameNum = 0;
		self->flags |= 0x20;
		if (self->health < 1000) {
			//we've been hit cap'n
			self->health = 1000;
			//make unshootable
			self->flags &= -0x21;
			//begin charging
			self->scriptState = 1;
			self->scriptTimer = 0;
		}
		break;
	case 1: //charge up
		self->scriptTimer++;
		self->frameNum = 1 + self->scriptTimer / 6;
		if (self->frameNum == 13) {
			//transition to firing
			self->scriptState = 2;
			self->scriptTimer = 0;
			CS_playSound(64, 1);
		}
		bwee = (self->frameNum - 1) / 4;
		if (self->scriptTimer & 2) {
			CS_playSound(59 + bwee, 1);
		}
		break;
	case 2: //fire
		self->scriptTimer++;
		if (self->scriptTimer%10 == 0) {
			CS_playSound(0x20, 1);
			CS_createBullet(34, self->xPos, self->yPos - 0x2000, 1);
		}
		if (self->scriptTimer > 50) {
			self->scriptState = 3;
			self->scriptTimer = 0;
		}
		self->frameNum = 13 + self->scriptTimer / 4 % 2;
		break;
	case 3: //cooldown
		self->scriptTimer++;
		self->frameNum = self->scriptTimer / 20 + 16;
		if (self->frameNum >= 20) {
			self->frameNum = 0;
			self->scriptTimer = 0;
			self->scriptState = 0;
		}
		break;
	}

	frameX = self->frameNum % 4;
	frameY = self->frameNum / 4;
	self->frameRect.left = 232 + frameX*SPRITE_W;
	self->frameRect.right = self->frameRect.left + SPRITE_W;
	self->frameRect.top = 112 + frameY*SPRITE_H;
	self->frameRect.bottom = self->frameRect.top + SPRITE_H;
}

void NPC_springpad(CS_ENTITY* self) {
	switch (self->scriptState) {
	case 0:
		if (isHitPlayer(self)) {
			//sproing
			self->scriptState = 1;
			self->scriptTimer = 0;
		}
		break;
	case 1:
		if (self->scriptTimer % 5 == 0) {
			//make a puf
			int pf = self->scriptTimer / 5;
			CS_createNPC(41, self->xPos, self->yPos - pf*0x1000, 0, -0x100, 0, 0, 0x80);
		}
		self->scriptTimer++;
		if (self->scriptTimer == 2) {
			CS_playSound(0x2E, 1);
			if (self->direction) {
				*CS_playerYvel = -0x1000;
				*CS_playerXvel = 0;
			} else {
				*CS_playerYvel = -0x5FF;
			}
		}
		if (self->scriptTimer == 15) {
			self->scriptState = 0;
		}
		break;
	}


	self->frameRect.left = 224;
	self->frameRect.top = 40;
	self->frameRect.right = 240;
	self->frameRect.bottom = 48;
}

void NPC_springpadFX(CS_ENTITY* self) {

	self->xPos += self->xVel; 
	self->yPos += self->yVel;

	self->frameTimer++;
	self->frameNum = self->frameTimer / 5;
	if (self->frameNum >= 4) {
		self->inUse = 0;
	}

	self->frameRect.left = 240;
	self->frameRect.right = 256;
	self->frameRect.top = 48 + self->frameNum * 8;
	self->frameRect.bottom = self->frameRect.top + 8;
}

void NPC_hitSwitch(CS_ENTITY* self) {

	if (self->health < 100) {
		//was hit
		self->health = 100;
		CS_runEvent(self->eventNum);
	}

	self->frameRect.left = 160;
	self->frameRect.top = 48;
	if (self->direction) {
		self->frameRect.left += 0x10;
	}
	self->frameRect.right = self->frameRect.left + 0x10;
	self->frameRect.bottom = self->frameRect.top + 0x10;
}

void NPC_gravekeeper(CS_ENTITY* self) {
	int frames[] = {
		0, 1, 0, 2, 3, 4, 5
	};
	int frame;
	switch (self->scriptState) {
	case 0:
		self->flags &= -0x21;
		self->scriptState = 1;
		self->damage = 0;
		self->hitRect.left = 0x800;
		self->hitRect.right = 0x800;
		goto LABEL_3;
	case 1:
	LABEL_3 :
		self->frameNum = 0;
			if (self->xPos - 0x10000 < *CS_playerX
				&& self->xPos + 0x10000 > *CS_playerX
				&& self->yPos - 0x6000 < *CS_playerY
				&& self->yPos + 0x4000 > *CS_playerY) {
				self->frameTimer = 0;
				self->scriptState = 2;
			}
			if (self->damageNumberTimer) {
				self->frameNum = 1;
				self->frameTimer = 0;
				self->scriptState = 2;
				self->flags &= -0x21;
			}
			if (*CS_playerX >= self->xPos)
				self->direction = 2;
			else
				self->direction = 0;
			break;
	case 2:
		++self->frameTimer;
		if (self->frameTimer > 6) {
			self->frameTimer = 0;
			++self->frameNum;
		}
		if (self->frameNum > 3)
			self->frameNum = 0;
		if (self->xPos - 0x2000 < *CS_playerX && self->xPos + 0x2000 > *CS_playerX) {
			self->scriptTimer = 0;
			self->scriptState = 3;
			self->flags |= 0x20u;
			CS_playSound(34, 1);
			if (self->direction) {
				self->xVel = 0x400;
				self->hitRect.right = 0x2400;
			} else {
				self->hitRect.left = 0x2400;
				self->xVel = -0x400;
			}
		}
		if (*CS_playerX >= self->xPos) {
			self->direction = 2;
			self->xVel = 256;
		} else {
			self->direction = 0;
			self->xVel = -256;
		}
		break;
	case 3:
		self->xVel = 0;
		++self->scriptTimer;
		if (self->scriptTimer > 40) {
			self->scriptTimer = 0;
			self->scriptState = 4;
			CS_playSound(106, 1);
		}
		self->frameNum = 4;
		break;
	case 4:
		self->damage = 2;
		++self->scriptTimer;
		if (self->scriptTimer > 2) {
			self->scriptTimer = 0;
			self->scriptState = 5;
		}
		self->frameNum = 5;
		break;
	case 5:
		self->frameNum = 6;
		self->damage = 0;
		++self->scriptTimer;
		if (self->scriptTimer > 60)
			self->scriptState = 0;
		break;
	default:
		break;
	}
	if (self->xVel < 0 && self->collision & 1)
		self->xVel = 0;
	if (self->xVel > 0 && self->collision & 4)
		self->xVel = 0;
	self->yVel += 32;
	if (self->xVel > 0x400)
		self->xVel = 0x400;
	if (self->xVel < -0x400)
		self->xVel = -0x400;
	if (self->yVel > 0x5FF)
		self->xVel = 0x5FF;
	if (self->yVel < -0x5FF)
		self->xVel = -0x5FF;
	self->xPos += self->xVel;
	self->yPos += self->yVel;

	frame = frames[self->frameNum];
	
	self->frameRect.left = 24 * frame;
	self->frameRect.top = self->direction ? 88 : 64;
	self->frameRect.right = self->frameRect.left + 24;
	self->frameRect.bottom = self->frameRect.top + 24;
}

void NPC_fanUp(CS_ENTITY* self) {
	int windPos;

	switch (self->scriptState) {
	case 0:
		if (self->direction == 2)
			self->scriptState = 2;
		else
			self->frameNum = 1;
	case 1:
		self->frameNum = 0;
		break;
	case 2:
		++self->frameTimer;
		if (self->frameTimer > 0) {
			self->frameTimer = 0;
			++self->frameNum;
		}
		if (self->frameNum > 2)
			self->frameNum = 0;
		if (*CS_playerX > self->xPos - 163840
			&& *CS_playerX < self->xPos + 163840
			&& *CS_playerY > self->yPos - 122880
			&& *CS_playerY < self->yPos + 122880
			&& CS_randInt(0, 5) == 1) {
			windPos = CS_randInt(-8, 8);
			CS_createNPC(199, self->xPos + (windPos << 9), self->yPos, 0, 0, 1, 0, 256);
		}
		if (*CS_playerX < self->xPos + 4096
			&& *CS_playerX > self->xPos - 4096
			&& *CS_playerY < self->yPos
			&& *CS_playerY > self->yPos - 49152) {
			if (*CS_playerYvel > -1500) {
				*CS_playerYvel -= 136;
			}
		}
	}

	RECT* frameRect = &self->frameRect;
	frameRect->left = 272 + self->frameNum * 16;
	frameRect->top = 136;
	frameRect->right = frameRect->left + 16;
	frameRect->bottom = 152;
}

void NPC_pedestalItem(CS_ENTITY* self) {
	self->frameRect.left = self->eventNum % 20 * 16;
	self->frameRect.top = 112;
	self->frameRect.right = self->frameRect.left + 16;
	self->frameRect.bottom = self->direction ? 128 : 112;
}


void NPC_treasureRain(CS_ENTITY* self) {
	int xOffset;
	int vals[] = { 1, 1, 1, 1, 1, 5, 5, 20 };
	switch (self->scriptState) {
	case 0:
		self->scriptTimer = 0;
		break;
	case 10:
		if (++self->scriptTimer > 1000) {
			self->scriptState = 0;
		}
		if (1) {
			xOffset = CS_randInt(-0xA000, 0xA000);
			CS_ENTITY* money = CS_createNPC(1, self->xPos + xOffset, self->yPos, 0, 0, 0, 0, 0);
			money->xp = vals[CS_randInt(0, 7)];
		}
	}
}
