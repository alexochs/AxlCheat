#include "MemoryManager.hpp"

MemoryManager g_MemoryManager;

unsigned long processId;

uintptr_t clientDll;
uintptr_t engineDll;

uintptr_t clientstate;

uintptr_t localPlayer;
uintptr_t entityList;

uintptr_t entities[64];

MemoryManager::MemoryManager()
{
	driver = KernelInterface("\\\\.\\axlkmd");
	
	processId = driver.GetProcessId();

	clientDll = driver.GetClientDll();
	engineDll = driver.GetEngineDll();
}