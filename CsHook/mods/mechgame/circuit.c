#include "stdafx.h"
#include "circuit.h"
#include "map.h"

#define CIRCUIT_SIZE 20
#define MAX_WIRES 40
Wire Wires[MAX_WIRES];
CircuitElement Circuits[CIRCUIT_SIZE];
int circuitLen = 0;
int wireLen = 0;

char isWirePowered(Wire* self) {
	int i;
	CircuitElement* input;
	for (i = 0;; i++) {
		input = self->inputs[i];
		if (!input) {
			break;
		}
		if (isPowered(input)) {
			return 1;
		}
	}
	return 0;
}

char isPowered_switch(CircuitElement* self, Wire* fromWire) {
	if (self->isOpenSwitch) {
		return 0;
	}
	if (self->input1 == fromWire) {
		return isPowered(self->input2);
	} else {
		return isPowered(self->input1);
	}
}

char isPowered_and(CircuitElement* self) {
	return isWirePowered(self->input1) && isWirePowered(self->input2);
}

char isPowered_xor(CircuitElement* self) {
	return isWirePowered(self->input1) ^ isWirePowered(self->input2);
}

char isPowered_or(CircuitElement* self) {
	return isWirePowered(self->input1) || isWirePowered(self->input2);
}

char isElementPowered(CircuitElement* self) {
	switch (self->type) {
	case CIRCUIT_TYPE_POWER_SOURCE:
		return 1;
	case CIRCUIT_TYPE_WIRE:
		return isPowered_wire(self);
	case CIRCUIT_TYPE_SWITCH:
		return isPowered_switch(self);
	case CIRCUIT_TYPE_ANDGATE:
		return isPowered_and(self);
	case CIRCUIT_TYPE_ORGATE:
		return isPowered_or(self);
	case CIRCUIT_TYPE_NANDGATE:
		return !isPowered_and(self);
	case CIRCUIT_TYPE_NORGATE:
		return !isPowered_or(self);
	case CIRCUIT_TYPE_XOR:
		return isPowered_xor(self);
	default:
		return 0;
	}
}

void initWire(Wire* self) {
	self->inputs = malloc(sizeof(CircuitElement*) * 10);
	self->inputsNext = 0;
	self->wireLocations = malloc(sizeof(Point) * 100);
}

void freeWire(Wire* self) {
	if (self->inputs) {
		free(self->inputs);
		self->inputs = 0;
	}
	if (self->wireLocations) {
		free(self->wireLocations);
		self->wireLocations = 0;
	}
}

void initElement(CircuitElement* self, int type, Point loc) {
	self->type = type;
	self->outputLoc = loc;
	self->isOpenSwitch = 0;
	self->powered = 0;
}


void resetCircuitNet() {
	for (int i = 0; i < CIRCUIT_SIZE; i++) {
		freeElement(&Circuits[i]);
	}
	memset(Circuits, 0, sizeof(CircuitElement)*CIRCUIT_SIZE);
	memset(Wires, 0, sizeof(Wire)*MAX_WIRES);
	circuitLen = 0;
}

char pointInList(Point* list, int x, int y) {
	for (int i = 0;; i++) {
		if (!(list[i].x | list[i].y)) {
			return 0;
		} else if (list[i].x == x && list[i].y == y) {
			return 1;
		}
	}
}

char isWire(int type) {
	return type == 0x11 || type == 0x46;
}

char isElement(int type) {
	return type >= 0x90 && type <= 0x9F;
}

void addInput(CircuitElement* self, int x, int y) {
	for (int i = 0; i < circuitLen; i++) {
		CircuitElement* checking = &Circuits[i];
		if (checking->outputLoc.x == x && checking->outputLoc.y == y) {
			self->inputs[self->inputsNext] = checking;
			self->inputsNext++;
			break;
		}
	}
}

void connectInputs(CircuitElement* self) {
	Point checked[100];
	Point* next;
	Point* checking;
	checking = checked;
	next = checked;
	int tileType;
	memset(checked, 0, sizeof(Point) * 100);
	// add input sources to list
	if (self->type == CIRCUIT_TYPE_SWITCH) {
		*next = self->outputLoc;
		next++;
	} else {
		for (int dx = -1; dx < 2; dx++) {
			for (int dy = -1; dy < 2; dy++) {
				tileType = getPxa(self->outputLoc.x + dx, self->outputLoc.y + dy);
				if (tileType == 0x12 || tileType == 0x13) {
					//its an input
					next->x = self->outputLoc.x + dx;
					next->y = self->outputLoc.y + dy;
					next++;
				}
			}
		}
	}
	while (checking->x | checking->y) {
		int px = checking->x;
		int py = checking->y; 
		Point dir[4] = { 
			{ .x = px - 1, .y = py },
			{ .x = px + 1, .y = py },
			{ .x = px, .y = py - 1 },
			{ .x = px, .y = py + 1 }
		};
		for (int i = 0; i < 4; i++) {
			Point* dp = &dir[i];
			if (!pointInList(checked, dp->x, dp->y)) {
				tileType = getPxa(dp->x, dp->y);
				if (isWire(tileType)) {
					*next = *dp;
					next++;
				} else if (isElement(tileType)) {
					addInput(self, dp->x, dp->y);
				}
			}
		}
		checking++;
	}
}

void connectAllInputs() {
	for (int i = 0; i < circuitLen; i++) {
		CircuitElement* next = &Circuits[i];
		connectInputs(next);
	}
}


int powerWire(unsigned int x, unsigned int y, unsigned int layer) {
	unsigned char type = getPxaLayer(x, y, layer);
	unsigned short tileId = getTile(x, y, layer);
	if (type == 0x46 || type == 0x45) {
		setTile(x, y, layer, tileId - 0x10);
		return 1;
	}
	return 0;
}

int solvePowerNet(int checkPoint, int writingPoint, int gridLayer) {
	POINT* checking = NULL;
	POINT* writing = &PowerNet[writingPoint];
	for (;;) {
		checking = &PowerNet[checkPoint];
		// quit looping if x and y are 0
		if (!(checking->x | checking->y)) {
			break;
		}
		if (powerWire(checking->x - 1, checking->y, gridLayer)) {
			writing->x = checking->x - 1;
			writing->y = checking->y;
			writing++;
			writingPoint++;
		}
		if (powerWire(checking->x + 1, checking->y, gridLayer)) {
			writing->x = checking->x + 1;
			writing->y = checking->y;
			writing++;
			writingPoint++;
		}
		if (powerWire(checking->x, checking->y - 1, gridLayer)) {
			writing->x = checking->x;
			writing->y = checking->y - 1;
			writing++;
			writingPoint++;
		}
		if (powerWire(checking->x, checking->y + 1, gridLayer)) {
			writing->x = checking->x;
			writing->y = checking->y + 1;
			writing++;
			writingPoint++;
		}
		checkPoint++;
	}
	return writingPoint;
}


int checkGate(int gateX, int gateY, int gridLayer, int writePoint, char checkNor) {
	unsigned char gateType = getPxaLayer(gateX, gateY, gridLayer);
	int powerCount = 0;
	int outX = 0, outY = 0;
	for (int dx = -1; dx < 2; dx++) {
		for (int dy = -1; dy < 2; dy++) {
			int tile = getPxaLayer(gateX + dx, gateY + dy, gridLayer);
			if (tile == 0x12) {
				powerCount++;
			}
			if (tile == 0x45) {
				outX = gateX + dx;
				outY = gateY + dy;
			}
		}
	}
	if (!(outX | outY)) {
		return 0;
	}
	char powered = 0;
	switch (gateType) {
	case 0x91: //xor
		if (powerCount == 1) {
			powered = powerWire(outX, outY, gridLayer);
		}
		break;
	case 0x90: //and
		if (powerCount == 2) {
			powered = powerWire(outX, outY, gridLayer);
		}
		break;
	case 0x92: // NOR
		if (powerCount == 0 && checkNor) {
			powered = powerWire(outX, outY, gridLayer);
		}
		break;
	}
	if (powered) {
		PowerNet[writePoint].x = outX;
		PowerNet[writePoint].y = outY;
	}

	return powered;
}


void checkGates() {
	char changed;
	int loops = 0;
	do {
		changed = 0;
		for (int i = 0; i < circuitLen; i++) {
			CircuitElement* c = &Circuits[i];
			char powered = isPowered(c);
			if (powered != c->powered) {
				c->powered = powered;
				changed = 1;
			}
		}
		if (++loops > 10) {
			loops /= 0;
		}
	} while (changed);
}

void createCircuitElement(int xPos, int yPos, int tileId) {
	Point p;
	p.x = xPos;
	p.y = yPos;
	int elementType;
	switch (tileId) {
	case 0x90:
		elementType = CIRCUIT_TYPE_SWITCH;
		break;
	case 0x91:
		elementType = CIRCUIT_TYPE_ANDGATE;
		break;
	case 0x92:
		elementType = CIRCUIT_TYPE_XOR;
		break;
	case 0x93:
		elementType = CIRCUIT_TYPE_NORGATE;
		break;
	case 0x94:
		elementType = CIRCUIT_TYPE_POWER_SOURCE;
		break;
	case 0x46:
		elementType = CIRCUIT_TYPE_WIRE;
	default:
		return;
	}
	if (elementType == CIRCUIT_TYPE_WIRE) {

	} else {
		initElement(&Circuits[circuitLen], elementType, p);
		++circuitLen;
	}
}
