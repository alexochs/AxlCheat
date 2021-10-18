#pragma once

#include "kernelinterface.hpp"

class MemoryManager
{
public:
	KernelInterface driver;

	MemoryManager();
};

extern MemoryManager g_MemoryManager;

