#include "stdafx.h"
#include "circuit.h"
#include "map.h"

#define CIRCUIT_SIZE 20
#define MAX_WIRES 40
Wire Wires[MAX_WIRES];
CircuitElement Circuits[CIRCUIT_SIZE];
int circuitLen = 0;
int wireLen = 0;

char isSwitchPowered(CircuitElement* self, Wire* fromWire);

char isWirePowered(Wire* self) {
	int i;
	CircuitElement* input;
	if (!self->checkedThisCycle) {
		self->checkedThisCycle = 1;
		for (i = 0;; i++) {
			input = self->inputs[i];
			if (!input) {
				break;
			}
			if (input->type == CIRCUIT_TYPE_SWITCH) {
				self->powered |= isSwitchPowered(input, self);
			} else {
				self->powered |= isElementPowered(input);
			}
		}
	}
	return self->powered;
}

char isSwitchPowered(CircuitElement* self, Wire* fromWire) {
	if (!self->checkedThisCycle) {
		self->checkedThisCycle = 1;
		for (int i = 0; i < 4; i++) {
			Wire* input = self->inputs[i];
			if (input && input != fromWire) {
				isWirePowered(input);
			}
		}
	}

	self->powered = 0;
	if (!self->isOpenSwitch) {
		for (int i = 0; i < 4; i++) {
			Wire* input = self->inputs[i];
			if (input) {
				self->powered |= input->powered;
			}
		}
	}
	return self->powered;
}

char isPowered_and(CircuitElement* self) {
	if (!self->checkedThisCycle) {
		self->checkedThisCycle = 1;
		char lhs = isWirePowered(self->inputs[0]);
		char rhs = isWirePowered(self->inputs[1]);
		self->powered = lhs && rhs;
	}
	return self->powered;
}

char isPowered_nand(CircuitElement* self) {
	if (!self->checkedThisCycle) {
		self->checkedThisCycle = 1;
		char lhs = isWirePowered(self->inputs[0]);
		char rhs = isWirePowered(self->inputs[1]);
		self->powered = !(lhs && rhs);
	}
	return self->powered;
}

char isPowered_xor(CircuitElement* self) {
	if (!self->checkedThisCycle) {
		self->checkedThisCycle = 1;
		char lhs = isWirePowered(self->inputs[0]);
		char rhs = isWirePowered(self->inputs[1]);
		self->powered = lhs ^ rhs;
	}
	return self->powered;
}


char isPowered_or(CircuitElement* self) {
	if (!self->checkedThisCycle) {
		self->checkedThisCycle = 1;
		self->powered = 0;
		for (int i = 0; i < 4; i++) {
			if (self->inputs[i]) {
				self->powered |= isWirePowered(self->inputs[i]);
			}
		}
	}
	return self->powered;
}
char isPowered_nor(CircuitElement* self) {
	if (!self->checkedThisCycle) {
		self->checkedThisCycle = 1;
		self->powered = 0;
		for (int i = 0; i < 4; i++) {
			if (self->inputs[i]) {
				self->powered |= isWirePowered(self->inputs[i]);
			}
		}
		self->powered = !self->powered;
	}
	return self->powered;
}

char isElementPowered(CircuitElement* self) {
	switch (self->type) {
	case CIRCUIT_TYPE_POWER_SOURCE:
		self->powered = 1;
		return 1;
	case CIRCUIT_TYPE_ANDGATE:
		return isPowered_and(self);
	case CIRCUIT_TYPE_ORGATE:
		return isPowered_or(self);
	case CIRCUIT_TYPE_NANDGATE:
		return isPowered_nand(self);
	case CIRCUIT_TYPE_NORGATE:
		return isPowered_nor(self);
	case CIRCUIT_TYPE_XOR:
		return isPowered_xor(self);
	case CIRCUIT_TYPE_SINK:
		return isPowered_or(self);
	default:
		return 0;
	}
}

void setWireTiles(Wire* self) {
	int target = getPxa(self->wireLocations[0].x, self->wireLocations[0].y);
	if (target == 0x46 && !self->powered || target == 0x11 && self->powered) {
		//assume the wires already ok
		return;
	}
	for (int i = 0; i < self->wireLocNext; i++) {
		Point p = self->wireLocations[i];
		int type = getPxa(p.x, p.y);
		int tile = getTile(p.x, p.y, getPhysicalLayer());
		if (self->powered && type == 0x46) {
			tile -= 0x10;
		} else if (!self->powered && type == 0x11) {
			tile += 0x10;
		}
		setTile(p.x, p.y, getPhysicalLayer(), tile);
	}
}

setSwitchTiles(CircuitElement* self) {
	unsigned char possibleType[] = {
		0x21, 0x40, 0x11, 0x10
	};
	int typeId = (self->powered << 1) | self->isOpenSwitch;
	setTile(self->outputLoc.x, self->outputLoc.y, getPhysicalLayer(), possibleType[typeId]);
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

void initWire(Wire* self, Point loc) {
	self->inputs = malloc(sizeof(CircuitElement*) * 10);
	memset(self->inputs, 0, sizeof(CircuitElement*) * 10);
	self->inputsNext = 0;
	self->wireLocations = malloc(sizeof(Point) * 100);
	memset(self->wireLocations, 0, sizeof(Point) * 100);
	self->wireLocNext = 0;
	self->wireInputsPlaceholder = malloc(sizeof(Point) * 10);
	memset(self->wireInputsPlaceholder, 0, sizeof(Point) * 10);
	self->placeholderNext = 0;
	self->wireLocations[0] = loc;
	self->wireLocNext = 1;
	self->powered = 0;
}

void freeWire(Wire* self) {
	if (!self) return;
	if (self->inputs) {
		free(self->inputs);
		self->inputs = 0;
	}
	if (self->wireLocations) {
		free(self->wireLocations);
		self->wireLocations = 0;
	}
	if (self->wireInputsPlaceholder) {
		free(self->wireInputsPlaceholder);
		self->wireInputsPlaceholder = 0;
	}
}

void initElement(CircuitElement* self, int type, Point loc) {
	self->type = type;
	self->outputLoc = loc;
	self->isOpenSwitch = 0;
	self->powered = 0;
	memset(self->inputs, 0, sizeof(Wire*) * 4);
}

CircuitElement* getElement(Point pos) {
	for (int i = 0; i < circuitLen; i++) {
		CircuitElement* element = &Circuits[i];
		if (element->outputLoc.x == pos.x && element->outputLoc.y == pos.y) {
			return element;
		}
	}
	return 0;
}

Wire* getWire(Point pos) {
	for (int i = 0; i < wireLen; i++) {
		Wire* wire = &Wires[i];
		if (pointInList(wire->wireLocations, pos.x, pos.y)) {
			return wire;
		}
	}
	return NULL;
}

void resetCircuitNet() {
	for (int i = 0; i < MAX_WIRES; i++) {
		freeWire(&Wires[i]);
	}
	memset(Circuits, 0, sizeof(CircuitElement)*CIRCUIT_SIZE);
	memset(Wires, 0, sizeof(Wire)*MAX_WIRES);
	circuitLen = 0;
	wireLen = 0;
}

char isWire(int type) {
	return type == 0x11 || type == 0x46;
}

char isElement(int type) {
	return type >= 0x90 && type <= 0x9F;
}

void setCircuitUnchecked() {
	for (int i = 0; i < circuitLen; i++) {
		Circuits[i].checkedThisCycle = 0;
	}
	for (int i = 0; i < wireLen; i++) {
		Wires[i].checkedThisCycle = 0; 
	}
}

void resetPowerState() {
	for (int i = 0; i < circuitLen; i++) {
		Circuits[i].powered = 0;
	}
	for (int i = 0; i < wireLen; i++) {
		Wires[i].powered = 0;
	}
}

void addElementInput(CircuitElement* self, Point inputPoint) {
	int px = inputPoint.x;
	int py = inputPoint.y;
	Point dir[4] = {
			{ .x = px - 1, .y = py },
			{ .x = px + 1, .y = py },
			{ .x = px, .y = py - 1 },
			{ .x = px, .y = py + 1 }
	};

	for (int i = 0; i < 4; i++) {
		int type = getPxa(dir[i].x, dir[i].y);
		if (isWire(type)) {
			Wire* w = getWire(dir[i]);
			if (w) {
				for (int j = 0; j < 4; j++) {
					if (!self->inputs[j]) {
						self->inputs[j] = w;
						break;
					}
				}
			}
		}
	}
}

void connectInputs(CircuitElement* self) {
	int tileType;
	// add input sources to list
	if (self->type == CIRCUIT_TYPE_SWITCH) {
		addElementInput(self, self->outputLoc);
	} else {
		for (int dx = -1; dx < 2; dx++) {
			for (int dy = -1; dy < 2; dy++) {
				Point dp = { .x = self->outputLoc.x + dx, .y = self->outputLoc.y + dy };
				tileType = getPxa(dp.x, dp.y);
				if (tileType == 0x12 || tileType == 0x13) {
					//its an input
					addElementInput(self, dp);
				}
			}
		}
	}
}

void connectWireInputs(Wire* self) {
	Point* checking = self->wireInputsPlaceholder;
	while (checking->x | checking->y) {
		CircuitElement* element = getElement(*checking);
		self->inputs[self->inputsNext] = element;
		self->inputsNext++;
		checking++;
	}
}

void connectAllInputs() {
	for (int i = 0; i < circuitLen; i++) {
		CircuitElement* next = &Circuits[i];
		connectInputs(next);
	}
	for (int i = 0; i < wireLen; i++) {
		Wire* next = &Wires[i];
		connectWireInputs(next);
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

void connectWirePoints(Wire* self) {
	Point* checking = self->wireLocations;
	Point* writing = &self->wireLocations[self->wireLocNext];
	int tileType;
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
			if (!pointInList(self->wireLocations, dp->x, dp->y)) {
				tileType = getPxa(dp->x, dp->y);
				if (isWire(tileType)) {
					*writing = *dp;
					writing++;
					self->wireLocNext++;
				} else if (isElement(tileType)) {
					self->wireInputsPlaceholder[self->placeholderNext] = *dp;
					self->placeholderNext++;
				}
			}
		}
		checking++;
	}
}

void checkGates() {
	resetPowerState();
	char changed;
	int loops = 0;
	do {
		changed = 0;
		for (int i = 0; i < circuitLen; i++) {
			CircuitElement* c = &Circuits[i];
			if (c->type == CIRCUIT_TYPE_SINK) {
				setCircuitUnchecked();
				char wasPowered = c->powered;
				isElementPowered(c);
				changed |= wasPowered != c->powered;
			}
		}
		if (++loops > 10) {
			loops /= 0;
		}
	} while (changed);

	for (int i = 0; i < wireLen; i++) {
		setWireTiles(&Wires[i]);
	}
	for (int i = 0; i < circuitLen; i++) {
		CircuitElement* c = &Circuits[i];
		if (c->type == CIRCUIT_TYPE_SWITCH) {
			setSwitchTiles(c);
		}
	}
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
		break;
	case 0x15:
		elementType = CIRCUIT_TYPE_SINK;
		break;
	default:
		return;
	}
	if (elementType == CIRCUIT_TYPE_WIRE) {
		initWire(&Wires[wireLen], p);
		connectWirePoints(&Wires[wireLen]);
		wireLen++;
	} else {
		initElement(&Circuits[circuitLen], elementType, p);
		++circuitLen;
	}
}

void toggleSwitch(int x, int y) {
	Point p = { x, y };
	CircuitElement* e = getElement(p);
	if (e) {
		e->isOpenSwitch = !e->isOpenSwitch;
		checkGates();
		setSwitchTiles(e);
	}
}
