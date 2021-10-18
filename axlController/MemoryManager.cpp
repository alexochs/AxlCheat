#include "MemoryManager.hpp"

MemoryManager g_MemoryManager;

MemoryManager::MemoryManager()
{
	driver = KernelInterface("\\\\.\\axlkmd");
}