#include "stdafx.h"
#include "cavestory.h"

// 0x4A6220 start of NPC data
CS_ENTITY* CS_npcTable = (CS_ENTITY*)0x4A6220;
// 0x4BF000 new mapdata
CS_MAPDATA* CS_mapdata = (CS_MAPDATA*)0x4BF000;
// old mapdata (useful for scratch or hooks)
int* CS_oldmapdata = (int*)0x4937b0;

int* CS_bgMode = (int*)0x499C88;
unsigned short* CS_mapWidth = (short*)0x49E586;
unsigned short* CS_mapHeight = (short*)0x49E588;
unsigned char** CS_mapTiles = (unsigned char**)0x49E480;
unsigned char* CS_pxaData = (unsigned char*)0x49E484;
int* CS_playerX = (int*)0x49E654;
int* CS_playerY = (int*)0x49E658;
int* CS_playerXvel = (int*)0x49E66C;
int* CS_playerYvel = (int*)0x49E670;
int* CS_soundPointers = (int*)0x4A5568;
char* CS_dataDirPath = (char*)0x49E220;

// 0x498548 NPC function pointer table
void(**CS_npcFuncTable)(CS_ENTITY*) = (VOID(**)(CS_ENTITY*))0x498548;
RECT* CS_fullScreenRect = (RECT*)0x48F91C;

int(*CS_randInt)(int, int) = (int(*)(int, int))0x40F350;
BOOL(*CS_peek_message)() = (BOOL(*)())0x40B340;
void(*CS_QuitMMTimer)() = (void(*)())0x41C250;
void(*CS_PlayOrganyaMusic)() = (void(*)())0x41C790;
unsigned char(*CS_GetTileType)(int, int) = (unsigned char(*)(int, int))0x4139A0;
void(*CS_putBitmap3)(RECT*, int, int, RECT*, int) 
					= (void(*)(RECT*, int, int, RECT*, int)) 0x40C3C0;

LONG CS_wndProc = 0x412CA0;
unsigned short* CS_orgTempo = (unsigned short*)0x4A4E18;

//0x421570 - destroy bitmap objects
//0x40B6C0(x) - destroy directdraw surfaces(?)
//0049E458 AppWinHandle
HWND* CS_appWinHandle = (HWND*)0x49E458;
//((VOID(*)(VOID))0x421570)();
//((VOID(*)(HWND))0x40B6C0)(appWinHandle);
