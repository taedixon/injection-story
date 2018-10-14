// CsHook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "cavestory.h"
#include "mods/mechgame/hooks.h"

#define EXTERN_DLL_EXPORT __declspec(dllexport)

EXTERN_DLL_EXPORT VOID HookMod() {

	/*
	 *	this is where we'll do the shenanigans
	 */
	hook_mechgame();
}	

