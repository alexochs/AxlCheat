#include "Aimbot.hpp"
#include "data.hpp"
#include "hazedumper.hpp"
#include <cmath>

void Aimbot::Run()
{
	if (bAimer) Aimer();
	if (bTrigger) Trigger();
}

Vector3 Aimbot::GetLocalPlayerHead()
{
	float_t localoriginX = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecOrigin, sizeof(float_t));
	float_t localoriginY = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecOrigin + sizeof(float_t), sizeof(float_t));
	float_t localoriginZ = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecOrigin + 2 * sizeof(float_t), sizeof(float_t));
	
	float_t localoffsetX = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecViewOffset, sizeof(float_t));
	float_t localoffsetY = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecViewOffset + sizeof(float_t), sizeof(float_t));
	float_t localoffsetZ = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecViewOffset + 2 * sizeof(float_t), sizeof(float_t));
	
	Vector3 local = { localoriginX + localoffsetX, localoriginY + localoffsetY, localoriginZ + localoffsetZ };

	return local;
}

int Aimbot::GetEntityIdByCrosshair()
{
	uint32_t localTeam = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, localPlayer + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));

	float minDiff = 999;
	int nearest = -1;

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

		uintptr_t boneMatrix = g_MemoryManager.driver.ReadVirtualMemory<uintptr_t>(processId, entities[i] + hazedumper::netvars::m_dwBoneMatrix, sizeof(uintptr_t));
		Vector3 head =
		{
			g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, boneMatrix + 0x30 * 8 + 0x0C, sizeof(float_t)),
			g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, boneMatrix + 0x30 * 8 + 0x1C, sizeof(float_t)),
			g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, boneMatrix + 0x30 * 8 + 0x2C, sizeof(float_t))
		};

		Vector3 dst = head - GetLocalPlayerHead();

		float yaw = atan2(dst.y, dst.x) * (180 / 3.14);
		float pitch = -atan(dst.z / sqrt(dst.x * dst.x + dst.y * dst.y)) * (180 / 3.14);
		float_t curYaw = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_angEyeAnglesY, sizeof(float_t));
		float_t curPitch = g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_angEyeAnglesX, sizeof(float_t));

		float diffLength = sqrt(pow(yaw - curYaw, 2) + pow(pitch - curPitch, 2));
		if (diffLength < minDiff && diffLength <= 3.5f) // in fov check
		{
			minDiff = diffLength;
			nearest = i;
		}
	}

	return nearest;
}

Vector3 Aimbot::CalculateAngles(Vector3 source, Vector3 target)
{
	Vector3 distance = target - source;
	return Vector3
	{ 
		-atan(distance.z / sqrt(distance.x * distance.x + distance.y * distance.y)) * (180.f / 3.14f),
		atan2(distance.y, distance.x) * (180.f / 3.14f),
		0 
	};
}

void Aimbot::NormalizeAngles(Vector3& angles)
{
	if (angles.x > 88.0f)
		angles.x = 88.0f;
	if (angles.x < -88.0f)
		angles.x = -88.0f;

	while (angles.y > 179.0f)
		angles.y -= 360.0f;
	while (angles.y < -179.0f)
		angles.y += 360.0f;
}

void Aimbot::Aimer()
{
	int target = GetEntityIdByCrosshair();
	if (target == -1) return;

	uintptr_t boneMatrix = g_MemoryManager.driver.ReadVirtualMemory<uintptr_t>(processId, entities[target] + hazedumper::netvars::m_dwBoneMatrix, sizeof(uintptr_t));
	Vector3 head =
	{
		g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, boneMatrix + 0x30 * 8 + 0x0C, sizeof(float_t)),
		g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, boneMatrix + 0x30 * 8 + 0x1C, sizeof(float_t)),
		g_MemoryManager.driver.ReadVirtualMemory<float_t>(processId, boneMatrix + 0x30 * 8 + 0x2C, sizeof(float_t))
	};

	Vector3 angles = CalculateAngles(GetLocalPlayerHead(), head);

	NormalizeAngles(angles);

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		g_MemoryManager.driver.WriteVirtualMemory<float_t>(processId, clientstate + hazedumper::signatures::dwClientState_ViewAngles, angles.x, sizeof(float_t));
		g_MemoryManager.driver.WriteVirtualMemory<float_t>(processId, clientstate + hazedumper::signatures::dwClientState_ViewAngles + sizeof(float_t), angles.y, sizeof(float_t));
		g_MemoryManager.driver.WriteVirtualMemory<float_t>(processId, clientstate + hazedumper::signatures::dwClientState_ViewAngles + 2 * sizeof(float_t), 0.f, sizeof(float_t));
		Sleep(250); // todo timing via timer
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