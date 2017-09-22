
void control_topdown(char canControl);
void drawPlayer(int camX, int camY);
void playerCalcFrame(char canControl);
void playerCheckTiles(void);

void drawPlayerLife(char show);
void drawPlayerArms(char show);
void drawMapName(int ignored);
void playerAct(int canControl);
void getCoin(int coinAmt);

void setPlayerSpecialState(int newState);
void resetTreasureCount();

extern int playerSpecialState;
extern int PrevTileFlags;
extern int* treasureActual;


enum PLAYER_SPECIAL_STATES {
	SSTATE_NONE = 0, SSTATE_SWING = 1, SSTATE_SWING1, SSTATE_SWING2, SSTATE_SWING_ENDLAG1, SSTATE_SWING_ENDLAG2,
	SSTATE_DIG, SSTATE_DIG_TRIGGER,
};
