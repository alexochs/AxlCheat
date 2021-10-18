#pragma once

#include <iostream>
#include <math.h>
#include "kernelinterface.hpp"
#include "hazedumper.hpp"
#include "Vector.hpp"
#include "data.hpp"

#define MAX_PLAYERS 64;

class HackManager
{
public:
	HackManager();

	void Run();
private:
	KernelInterface driver;
	DWORD processId;

	DWORD clientDll;
	DWORD engineDll;

	uintptr_t clientstate;

	uintptr_t localPlayer;
	uintptr_t entityList;

	uintptr_t entities[64];

	bool bNoFlash = true;
	bool bRadar = true;
	bool bAimbot = true;
	bool bGlow = true;
	bool bTrigger = true;
	bool bBhop = true;

	bool bDebug = false;
	
	void Init();
	void ReadAddresses();
	void ReadEntities();
	void NoFlash();
	void Radar();
	void Aimbot();
	void Glow();
	void Trigger();
	void Bhop();

	void Debug();
};

