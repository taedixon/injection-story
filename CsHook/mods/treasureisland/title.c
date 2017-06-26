#include "stdafx.h"
#include "cavestory.h"
#include "title.h"

// this one's a bit tricky because the title screen
// actually contains the hook for this DLL. so,
// in the ASM make sure it calls getVersion() before
// jumping to the hook
int doTitle(HWND window) {
	*CS_gameState = 0;
	RECT frameRect = { 0, 0, 320, 240 };
	// set the song
	CS_playSong(24);

	//enter the title screen loop
	int inputTimer = 0;
	while (1) {
		CS_updateKeys();
		if (inputTimer < 10) {
			++inputTimer;
		} else {
			if (0x8000 & *CS_keyHeld) {
				//trigger esc menu
				int escResult = CS_escMenuLoop(window);
				if (!escResult) {
					return 0;
				}
			}
			if (*CS_keyPressed & *CS_keyJump) {
				// select menu option
				CS_playSound(18, 1);
				CS_playSong(0);
				break;
			}
		}



		//handle title control logic

		//draw the title
		CS_putBitmap3(CS_fullScreenRect, 0, 0, &frameRect, CS_BM_TITLE);
		//update screen
		if (!CS_flipScreen(window)) {
			return 0;
		}
	}

	return 3;
}
