#include "stdafx.h"
// return 1 to finish command and halt parsing
// return 0 to finish command and continue parsing
// return -1 to continue searching for existing command
int tscHook() {
	char* scriptPointer = *CS_scriptPointer + *CS_scriptOffset;
	int currentCommand = *(int*)scriptPointer;
	if (currentCommand == *(int*)"<END") {
		// we "hijack" this command in the vanilla parser to inject our hook,
		// so we must duplicate it here.
		*CS_gameState |= 3;
		*CS_tscState = 0;
		*CS_currentFacepic = 0;
		*CS_playerStateFlags &= -2;
		return 1;
	} else if (currentCommand == *(int*)"<NAM") {
		char* nextCommand;
		char namebuf[32];
		int count;
		*CS_scriptOffset += 4;
		scriptPointer += 4;
		nextCommand = strchr(scriptPointer, '<');
		if (nextCommand != NULL) {
			count = nextCommand - scriptPointer;
			*CS_scriptOffset += count;
			if (count > 31) count = 31;
			strncpy(namebuf, scriptPointer, count);
			namebuf[count] = 0;
			CS_setMapName(namebuf);
		}
		return 0;
	}

	return -1;
}
