#include "stdafx.h"
void weapon_shovel(int lvl);

void bullet_shovel(CS_BULLET* self);
void drawBullet_debug(CS_BULLET* self, int camX, int camY);

extern void(*bulletFuncs[])(CS_BULLET*);
