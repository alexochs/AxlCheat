#pragma once

#include "MemoryManager.hpp"

class Intel
{
public:
	Intel() {};

	void Run();

private:
	bool bRadar = true;
	bool bGlow = true;

	void Radar();
	void Glow();
};

