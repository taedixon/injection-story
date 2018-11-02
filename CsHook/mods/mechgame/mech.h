#include "stdafx.h"

typedef struct ModuleAttrib {
	int id;
	int powerCost;
	void(*modifyOnEquip)(struct _MechAttrib* mech, char isUnequip);
	void(*modifyPerFrame)(struct _MechAttrib* mech);
	RECT iconFrame;
} MODULE_ATTRIB;


typedef enum mechState {
	NORMAL,
	PREJUMP
} MECHSTATE;

typedef struct _MechAttrib {
	int init;
	int* xPos;
	int* yPos;
	int* xVel;
	int* yVel;
	int thrust;
	int speedCap;
	int friction;
	int jumpForce;
	int hoverForce;
	int gravity;
	char hoverOn;
	int hoverCost;

	int armourMax;
	int armourCurrent;

	MECHSTATE controlState;
	int scriptState;
	int scriptTimer1;
	int scriptTimer2;

	int shieldMax;
	int shieldCurrent;
	int shieldRechargeRate;

	int batteryMax;
	int batteryCurrent;
	int batteryRechargeRate;

} MechAttrib;

void initMech();
void controlMech(char canControl);

void drawMechUI();
void calcFrameMech();
