#include "stdafx.h"
#include "cavestory.h"
#include "title.h"
#include "player.h"

// this one's a bit tricky because the title screen
// actually contains the hook for this DLL. so,
// in the ASM make sure it calls getVersion() before
// jumping to the hook
int doTitle(HWND window) {
	const int ANIM_SPEED = 8;
	const int CURSOR_FRAMES = 6;
	resetTreasureCount();

	*CS_gameState = 0;
	RECT frameRect = { 0, 0, 320, 240 };
	RECT cursorRect = { 0, 48, 16, 64 };
	// set the song
	CS_playSong(24);
	int cursorFrameTimer = 0;
	int cursorY;

	*CS_titleCursor = CS_doesProfileExist();

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
			//handle title control logic
			if (*CS_keyPressed & *CS_keyJump) {
				// select menu option
				CS_playSound(18, 1);
				CS_playSong(0);
				break;
			}
			if (*CS_keyPressed & (*CS_keyUp | *CS_keyDown)) {
				*CS_titleCursor ^= 1;
				CS_playSound(1, 1);
			}
		}

		if (++cursorFrameTimer >= (ANIM_SPEED*CURSOR_FRAMES)) {
			cursorFrameTimer = 0;
		}


		//draw the title
		CS_putBitmap3(CS_fullScreenRect, 0, 0, &frameRect, CS_BM_TITLE);

		//draw the cursor
		cursorRect.left = (cursorFrameTimer / ANIM_SPEED) * 0x10;
		cursorRect.right = cursorRect.left + 0x10;
		cursorY = 148 + *CS_titleCursor * 32;

		CS_putBitmap3(CS_fullScreenRect, 178, cursorY, &cursorRect, CS_BM_SYM);

		//update screen
		if (!CS_flipScreen(window)) {
			return 0;
		}
	}

	return 3; // enter GAME mode on exit
}
