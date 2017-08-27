#include "stdafx.h"
#include "cavestory.h"

// 0x4A6220 start of NPC data
CS_ENTITY* CS_npcTable = (CS_ENTITY*)0x4A6220;
// 0x4BF000 new mapdata
CS_MAPDATA* CS_mapdata = (CS_MAPDATA*)0x4BF000;
// old mapdata (useful for scratch or hooks)
int* CS_oldmapdata = (int*)0x4937b0;

char* CS_flagdata = (char*)0x49DDA0;

int* CS_lastUsedNumSlot = (int*)0x4A5F90;
CS_NUM_OBJ* CS_numbers = (CS_NUM_OBJ*)0x4A5F98;

//map
int* CS_bgMode = (int*)0x499C88;
unsigned short* CS_mapWidth = (short*)0x49E586;
unsigned short* CS_mapHeight = (short*)0x49E588;
unsigned char** CS_mapTiles = (unsigned char**)0x49E480;
unsigned char* CS_pxaData = (unsigned char*)0x49E484;
int* CS_globalWaterDepth = (int*)0x499C90;
int* CS_camX = (int*)0x49E1C8;
int* CS_camY = (int*)0x49E1CC;

//player
int* CS_playerDirection = (int*)0x49E640;
int* CS_playerFacingUp = (int*)0x49E644;
int* CS_playerFacingDown = (int*)0x49E648;
int* CS_playerFrameNum = (int*)0x49E678;
int* CS_playerEquipFlags = (int*)0x49E650;
int* CS_playerStateFlags = (int*)0x49E638;
int* CS_playerTileFlags = (int*)0x49E63C;
int* CS_playerX = (int*)0x49E654;
int* CS_playerY = (int*)0x49E658;
int* CS_playerXvel = (int*)0x49E66C;
int* CS_playerYvel = (int*)0x49E670;
int* CS_playerCamX = (int*)0x49E65C;
int* CS_playerCamY = (int*)0x49E660;
int* CS_playerCamFocusX = (int*)0x49E664;
int* CS_playerCamFocusY = (int*)0x49E668;
RECT* CS_playerFrameRect = (RECT*)0x49E69C;
RECT* CS_playerHitRect = (RECT*)0x49E67C;
RECT* CS_playerSizeRect = (RECT*)0x49E68C;
RECT* CS_playerWepRect = (RECT*)0x49E6AC;
CS_WEAPON* CS_playerWeapons = (CS_WEAPON*)0x499BC8;
int* CS_playerSelWeapon = (int*)0x499C68;
int* CS_playerInvulnTimer = (int*)0x49E6C8;
short* CS_playerCurrentHealth = (short*)0x49E6CC;
short* CS_playerMaxHealth = (short*)0x49E6D0;
int* CS_expToGain = (int*)0x49E6C4;

//sound
int* CS_soundPointers = (int*)0x4A5568;

//system
HWND* CS_appWinHandle = (HWND*)0x49E458;
LONG CS_wndProc = 0x412CA0;
RECT* CS_fullScreenRect = (RECT*)0x48F91C;
char* CS_dataDirPath = (char*)0x49E220;
int* CS_lastKeyHeld = (int*)0x49E218;
int* CS_keyHeld = (int*)0x49E210;
int* CS_keyPressed = (int*) 0x49E214;
int* CS_controlMode = (int*)0x49E64C;
int* CS_gameState = (int*)0x49E1E8;
int* CS_isShowingMapName = (int*)0x49E590;
int* CS_mapNameTimer = (int*)0x49E594;
//this value determines whether to load or begin a new profile
int* CS_titleCursor = (int*)0x49E1E4;
//ddraw
LPDIRECTDRAWSURFACE7* CS_imgSurfaces = (LPDIRECTDRAWSURFACE7*)0x49D388;

//script
char** CS_scriptPointer = (char**)0x4A5AD8;
unsigned char* CS_tscState = (unsigned char*)0x4A5ADC;
int* CS_scriptOffset = (int*)0x4A5AE0;
int* CS_currentFacepic = (int*)0x4A5B0C;

//keys
//int* CS_keyJump = (int*)0x493610; (dup?)
//int* CS_keyShoot = (int*)0x493614; (dup?)
int* CS_keyNextWep = (int*)0x493618;
int* CS_keyPrevWep = (int*)0x49361C;
int* CS_keyMenu = (int*)0x493620;
int* CS_keyMap = (int*)0x493624;
int* CS_keyJump = (int*)0x493628;
int* CS_keyShoot = (int*)0x49362C;
int* CS_keyLeft = (int*)0x493630;
int* CS_keyUp = (int*)0x493634;
int* CS_keyRight = (int*)0x493638;
int* CS_keyDown = (int*)0x49363C;

//custom/special
int* CS_treasureActual = (int*)0x49E370;
int* CS_treasureShown = (int*)0x49E374;


unsigned short* CS_orgTempo = (unsigned short*)0x4A4E18;

// 0x498548 NPC function pointer table
void(**CS_npcFuncTable)(CS_ENTITY*) = (void(**)(CS_ENTITY*))0x498548;

int(*CS_randInt)(int, int) = (int(*)(int, int))0x40F350;
BOOL(*CS_flipScreen)(HWND) = (BOOL(*)(HWND))0x40B340;
void(*CS_QuitMMTimer)() = (void(*)())0x41C250;
void(*CS_PlayOrganyaMusic)() = (void(*)())0x41C790;
void(*CS_playSong)(int) = (void(*)(int))0x420EE0;
void(*CS_playSound)(int, int) = (void(*)(int, int))0x420640;
unsigned char(*CS_GetTileType)(int, int) = (unsigned char(*)(int, int))0x4139A0;
void(*CS_putBitmap3)(RECT*, int, int, RECT*, int) 
					= (void(*)(RECT*, int, int, RECT*, int)) 0x40C3C0;
void(*CS_surfaceBlt)(int, int, RECT*, int, int) = (void(*)(int, int, RECT*, int, int))0x40C7A0;
void(*CS_fillRect)(RECT*, int) = (void(*)(RECT*, int)) 0x40C9E0;
void(*CS_fillSurfaceRect)(RECT*, int, int) = (void(*)(RECT*, int, int)) 0x40CA80;
void(*CS_updateKeys)() = (void(*)())0x4122E0;
int(*CS_escMenuLoop)(HWND window) = (int(*)(HWND))0x40DD70;
void(*CS_setMapName)(char* name) = (void(*)(char*))0x4140F0;
void(*CS_runEvent)(int eve) = (void(*)(int))0x421990;
int(*CS_atoi)(int scriptOffset) = (int(*)(int))0x421900;

void(*CS_createBullet)(int id, int x, int y, int direction) = (void(*)(int, int, int, int))0x403F80;
void(*CS_createNPC)(int, int, int, int, int, int, int, int)
					= (void(*)(int, int, int, int, int, int, int, int))0x46EFD0;
void(*CS_createEffect)(int, int, int, int) = (void(*)(int, int, int, int))0x40AC90;
int(*CS_doesProfileExist)() = (int(*)())0x41CFC0;
void(*CS_createNumberObject)(int*, int*, int) = (void(*)(int*, int*, int))0x425BF0;


void(*CS_playerAgility)(int) = (void(*)(int))0x4156C0;
int(*CS_playerHitSquareBlock)(int, int) = (int(*)(int, int))0x416E30;
int(*CS_playerHitSlopeFloorA)(int, int) = (int(*)(int, int))0x4171D0;
int(*CS_playerHitSlopeFloorB)(int, int) = (int(*)(int, int))0x4172E0;
int(*CS_playerHitSlopeFloorC)(int, int) = (int(*)(int, int))0x4173F0;
int(*CS_playerHitSlopeFloorD)(int, int) = (int(*)(int, int))0x417500; 
int(*CS_playerHitSlopeRoofA)(int, int) = (int(*)(int, int))0x417610;
int(*CS_playerHitSlopeRoofB)(int, int) = (int(*)(int, int))0x417720;
int(*CS_playerHitSlopeRoofC)(int, int) = (int(*)(int, int))0x417830;
int(*CS_playerHitSlopeRoofD)(int, int) = (int(*)(int, int))0x417940;
int(*CS_playerHitWater)(int, int) = (int(*)(int, int))0x417A50;
int(*CS_playerHitSpike)(int, int) = (int(*)(int, int))0x417AE0;
int(*CS_playerHitSpikeWater)(int, int) = (int(*)(int, int))0x417B70;
int(*CS_playerHitWindLeft)(int, int) = (int(*)(int, int))0x417C00;
int(*CS_playerHitWindUp)(int, int) = (int(*)(int, int))0x417C90;
int(*CS_playerHitWindRight)(int, int) = (int(*)(int, int))0x417D20;
int(*CS_playerHitWindDown)(int, int) = (int(*)(int, int))0x41DB0;

void(*CS_killNpc)(CS_ENTITY* npc, int shouldVanish) = (void(*)(CS_ENTITY*, int))0x471B80;


//0x421570 - destroy bitmap objects
//((VOID(*)(VOID))0x421570)();
//((VOID(*)(HWND))0x40B6C0)(appWinHandle);
