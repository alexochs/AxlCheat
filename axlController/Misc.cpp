#include "Misc.hpp"
#include "hazedumper.hpp"
#include "data.hpp"

void Misc::Run()
{
	if (bNoFlash) NoFlash();
	if (bBhop) Bhop();
}

void Misc::NoFlash()
{
	g_MemoryManager.driver.WriteVirtualMemory<uint32_t>(processId, localPlayer + hazedumper::netvars::m_flFlashMaxAlpha, (uint32_t)0, sizeof(uint32_t));
}

void Misc::Bhop()
{
	int32_t flags = g_MemoryManager.driver.ReadVirtualMemory<int32_t>(processId, localPlayer + hazedumper::netvars::m_fFlags, sizeof(int32_t));
	uint32_t buffer;
	if (flags & 1) buffer = 6;
	else buffer = 4;

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		g_MemoryManager.driver.WriteVirtualMemory<uint32_t>(processId, clientDll + hazedumper::signatures::dwForceJump, buffer, sizeof(uint32_t));
}