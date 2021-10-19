#include "Intel.hpp"
#include "data.hpp"
#include "hazedumper.hpp"
#include <cmath>

void Intel::Run()
{
	if (bRadar) Radar();
	if (bGlow) Glow();
}

void Intel::Radar()
{
	for (int i = 1; i < 64; i++)
	{
		if (entities[i]) g_MemoryManager.driver.WriteVirtualMemory<bool>(processId, entities[i] + hazedumper::netvars::m_bSpotted, true, sizeof(bool));
	}
}

void Intel::Glow()
{
	uint32_t localTeam = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, localPlayer + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));

	for (int i = 1; i < 64; i++)
	{
		uint32_t pPlayer = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, clientDll + hazedumper::signatures::dwEntityList + i * 0x10, sizeof(uint32_t));

		if (!pPlayer)
			continue;

		int32_t hp = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, pPlayer + hazedumper::netvars::m_iHealth, sizeof(int32_t));
		if (hp < 1)
			continue;

		uint32_t entityTeam = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, pPlayer + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));
		if (localTeam == entityTeam)
			continue;

		uint32_t glowIndex = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, pPlayer + hazedumper::netvars::m_iGlowIndex, sizeof(uint32_t));
		uint32_t glowObject = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, clientDll + hazedumper::signatures::dwGlowObjectManager, sizeof(uint32_t));

		g_MemoryManager.driver.WriteVirtualMemory<float_t>(processId, glowObject + glowIndex * 0x38 + 0x8, 1 - ((float_t)hp / 100), sizeof(float_t));
		g_MemoryManager.driver.WriteVirtualMemory<float_t>(processId, glowObject + glowIndex * 0x38 + 0xC, (float_t)hp / 100, sizeof(float_t));
		g_MemoryManager.driver.WriteVirtualMemory<float_t>(processId, glowObject + glowIndex * 0x38 + 0x10, 0.f, sizeof(float_t));
		g_MemoryManager.driver.WriteVirtualMemory<float_t>(processId, glowObject + glowIndex * 0x38 + 0x14, 1.f, sizeof(float_t));
		g_MemoryManager.driver.WriteVirtualMemory<bool>(processId, glowObject + glowIndex * 0x38 + 0x28, true, sizeof(bool));
		g_MemoryManager.driver.WriteVirtualMemory<bool>(processId, glowObject + glowIndex * 0x38 + 0x29, false, sizeof(bool));
	}
}