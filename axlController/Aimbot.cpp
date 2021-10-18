#include "Aimbot.hpp"
#include "data.hpp"
#include "hazedumper.hpp"
#include "Vector.hpp"
#include <cmath>

void Aimbot::Run()
{
	if (bAimer) Aimer();
	if (bTrigger) Trigger();
}

void Aimbot::Aimer()
{
	float_t localoriginX = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecOrigin, sizeof(float_t));
	float_t localoriginY = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecOrigin + sizeof(float_t), sizeof(float_t));
	float_t localoriginZ = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecOrigin + 2 * sizeof(float_t), sizeof(float_t));
	float_t localoffsetX = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecViewOffset, sizeof(float_t));
	float_t localoffsetY = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecViewOffset + sizeof(float_t), sizeof(float_t));
	float_t localoffsetZ = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecViewOffset + 2 * sizeof(float_t), sizeof(float_t));
	Vector3 local = { localoriginX + localoffsetX, localoriginY + localoffsetY, localoriginZ + localoffsetZ };
	uint32_t localTeam = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, localPlayer + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));

	float aimPitch = 0;
	float aimYaw = 420;
	float minDiff = 999;

	for (int i = 0; i < 64; i++)
	{
		if (!entities[i])
			continue;

		int32_t hp = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, entities[i] + hazedumper::netvars::m_iHealth, sizeof(int32_t));
		if (hp < 1)
			continue;

		uint32_t entityTeam = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, entities[i] + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));
		if (localTeam == entityTeam)
			continue;

		/*float_t originX = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, entities[i] + hazedumper::netvars::m_vecOrigin, sizeof(float_t));
		float_t originY = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, entities[i] + hazedumper::netvars::m_vecOrigin + sizeof(float_t), sizeof(float_t));
		float_t originZ = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, entities[i] + hazedumper::netvars::m_vecOrigin + 2*sizeof(float_t), sizeof(float_t));
		float_t offsetX = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, entities[i] + hazedumper::netvars::m_vecViewOffset, sizeof(float_t));
		float_t offsetY = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, entities[i] + hazedumper::netvars::m_vecViewOffset + sizeof(float_t), sizeof(float_t));
		float_t offsetZ = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, entities[i] + hazedumper::netvars::m_vecViewOffset + 2 * sizeof(float_t), sizeof(float_t));*/
		uintptr_t boneMatrix = g_MemoryManager.driver.ReadVirtualMemory<uintptr_t>(processId, entities[i] + hazedumper::netvars::m_dwBoneMatrix, sizeof(uintptr_t));
		Vector3 head =
		{
			g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, boneMatrix + 0x30 * 8 + 0x0C, sizeof(float_t)),
			g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, boneMatrix + 0x30 * 8 + 0x1C, sizeof(float_t)),
			g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, boneMatrix + 0x30 * 8 + 0x2C, sizeof(float_t))
		};

		Vector3 dst = head - local;

		float yaw = atan2(dst.y, dst.x) * (180 / 3.14);
		float pitch = -atan(dst.z / sqrt(dst.x * dst.x + dst.y * dst.y)) * (180 / 3.14);
		float_t curYaw = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_angEyeAnglesY, sizeof(float_t));
		float_t curPitch = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_angEyeAnglesX, sizeof(float_t));
		/*float diff[2] = {pitch - curPitch, yaw - curYaw};

		for (auto i = 0; i < 2; i++) {
			while (diff[i] < -180.0f) diff[i] += 360.0f;
			while (diff[i] > 180.0f) diff[i] -= 360.0f;
		}*/

		float diffLength = sqrt(pow(yaw - curYaw, 2) + pow(pitch - curPitch, 2));
		if (diffLength < minDiff)
		{
			minDiff = diffLength;
			aimPitch = pitch;
			aimYaw = yaw;
		}
	}

	if (aimPitch == 420) return;

	if (aimPitch > 88.0f)
		aimPitch = 88.0f;
	if (aimPitch < -88.0f)
		aimPitch = -88.0f;

	while (aimYaw > 179.0f)
		aimYaw -= 359.0f;
	while (aimYaw < -179.0f)
		aimYaw += 359.0f;

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && minDiff <= 3.5)
	{
		g_MemoryManager.driver.WriteVirtualMemory<float_t>(processId, clientstate + hazedumper::signatures::dwClientState_ViewAngles, aimPitch, sizeof(float_t));
		g_MemoryManager.driver.WriteVirtualMemory<float_t>(processId, clientstate + hazedumper::signatures::dwClientState_ViewAngles + sizeof(float_t), aimYaw, sizeof(float_t));
		g_MemoryManager.driver.WriteVirtualMemory<float_t>(processId, clientstate + hazedumper::signatures::dwClientState_ViewAngles + 2 * sizeof(float_t), 0.f, sizeof(float_t));
		Sleep(250);
	}
}

void Aimbot::Trigger()
{
	uintptr_t pLocalplayer = g_MemoryManager.driver.ReadVirtualMemory<uintptr_t>(processId, clientDll + hazedumper::signatures::dwLocalPlayer, sizeof(uintptr_t));
	uint32_t localTeam = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, pLocalplayer + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));
	uint32_t inCrosshairId = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, pLocalplayer + hazedumper::netvars::m_iCrosshairId, sizeof(uint32_t));

	if (inCrosshairId > 0 && inCrosshairId < 64)
	{
		uintptr_t pPlayer = g_MemoryManager.driver.ReadVirtualMemory<uintptr_t>(processId, clientDll + hazedumper::signatures::dwEntityList + (inCrosshairId - 1) * 0x10, sizeof(uintptr_t));
		uint32_t inCrosshairTeam = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, pPlayer + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));
		int32_t inCrosshairHealth = g_MemoryManager.driver.ReadVirtualMemory<int32_t>(processId, pPlayer + hazedumper::netvars::m_iHealth, sizeof(int32_t));

		if (localTeam != inCrosshairTeam && inCrosshairHealth > 0 && (GetKeyState(VK_MENU) & 0x8000))
		{
			g_MemoryManager.driver.WriteVirtualMemory<uint32_t>(processId, clientDll + hazedumper::signatures::dwForceAttack, 1, sizeof(uint32_t));
			Sleep(1);
			g_MemoryManager.driver.WriteVirtualMemory<uint32_t>(processId, clientDll + hazedumper::signatures::dwForceAttack, 4, sizeof(uint32_t));
		}
	}
}