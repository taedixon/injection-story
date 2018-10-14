#include "stdafx.h"

typedef struct ModuleAttrib {
	int id;
	int powerCost;
	void(*modifyOnEquip)(MECH_ATTRIB* mech, char isUnequip);
	void(*modifyPerFrame)(MECH_ATTRIB* mech);
	RECT iconFrame;
} MODULE_ATTRIB;


typedef struct MechAttrib {
	int xPos;
	int yPos;
	int xVel;
	int yVel;
	int thrust;
	int speedCap;
	int friction;
	int jumpForce;
	int hoverForce;

	int armourMax;
	int armourCurrent;

	int shieldMax;
	int shieldCurrent;
	int shieldRechargeRate;

	int batteryMax;
	int batteryCurrent;
	int batteryRechargeRate;

} MECH_ATTRIB;
