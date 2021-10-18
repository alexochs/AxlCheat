#pragma once

#include <iostream>
#include <math.h>
#include "hazedumper.hpp"
#include "Vector.hpp"
#include "data.hpp"
#include <Windows.h>
#include "MemoryManager.hpp"
#include "Misc.hpp"
#include "Intel.hpp"
#include "Aimbot.hpp"

#define MAX_PLAYERS 64;

class HackManager
{
public:
	HackManager();

	void Run();
private:	
	Misc misc;
	Intel intel;
	Aimbot aimbot;

	bool bDebug = false;
	
	void Init();
	void ReadAddresses();
	void ReadEntities();

	void Debug();
};

