#include "stdafx.h"
void weapon_shovel(int lvl);
void shovel_do_attack();

void bullet_shovel(CS_BULLET* self);
void drawBullet(int camX, int camY);
void drawBullet_debug(CS_BULLET* self, int camX, int camY);

extern void(*bulletFuncs[])(CS_BULLET*);
