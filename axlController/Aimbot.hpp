#pragma once

#include "MemoryManager.hpp"

class Aimbot
{
public:
	Aimbot() {};

	void Run();

private:
	bool bAimer = true;
	bool bTrigger = true;

	void Aimer();
	void Trigger();
};

