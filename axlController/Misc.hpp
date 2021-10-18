#pragma once

#include "MemoryManager.hpp"

class Misc
{
public:
	Misc() {};

	void Run();

private:
	bool bNoFlash = true;
	bool bBhop = true;

	void NoFlash();
	void Bhop();
};

