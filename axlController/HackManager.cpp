#include "HackManager.hpp"

HackManager::HackManager()
{
	Init();
	ReadAddresses();
}

void HackManager::Run()
{
	while (true)
	{
		ReadAddresses();
		ReadEntities();

		if (bNoFlash) NoFlash();
		if (bRadar) Radar();
		if (bAimbot) Aimbot();
		if (bGlow) Glow();
		if (bTrigger) Trigger();
		if (bBhop) Bhop();

		if (bDebug) Debug();

		Sleep(10);
	}
}

void HackManager::Init()
{
	std::cout <<
		"###################################################" << std::endl <<
		"###   AxlCheat - Kernel Edition (Early Alpha)   ###" << std::endl <<
		"###################################################\n" << std::endl;

	std::cout << "Initializing...\n" << std::endl;


	std::cout << "Loading Driver..." << std::endl;

	/*std::cout << "Setting Module: ac_client.exe" << std::endl;
	if (Driver.SetModulePathSubstring(L"\\AssaultCube\\bin_win32\\ac_client.exe lol"))
		std::cout << "Successfully initialized! \n" << std::endl;
	else
		std::cout << "Fail :( \n" << std::endl;*/;

	std::cout << "\nRequesting Process ID..." << std::endl;
	processId = g_MemoryManager.driver.GetProcessId();
	if (processId)	std::cout << "Process ID: " << processId << std::endl;
	else			std::cout << "Fail :(" << std::endl;

	std::cout << "\nRequesting Client DLL..." << std::endl;
	clientDll = g_MemoryManager.driver.GetClientDll();
	if (clientDll)	std::cout << "Client DLL: " << clientDll << std::endl;
	else			std::cout << "Fail :(" << std::endl;

	std::cout << "\nRequesting Engine DLL..." << std::endl;
	engineDll = g_MemoryManager.driver.GetEngineDll();
	if (engineDll)	std::cout << "Engine DLL: " << engineDll << std::endl;
	else			std::cout << "Fail :(" << std::endl;

	bool bInit = false;
	while (!bInit)
	{
		while (!processId)
		{
			std::cout << "\nRequesting Process ID..." << std::endl;

			processId = g_MemoryManager.driver.GetProcessId();
			if (processId)	std::cout << "Process ID: " << processId << std::endl;
			else			std::cout << "Fail :(\nTrying again in 3 seconds..." << std::endl;

			Sleep(3000);
		}

		while (!clientDll)
		{
			std::cout << "\nRequesting Client DLL..." << std::endl;
			clientDll = g_MemoryManager.driver.GetClientDll();
			if (clientDll)	std::cout << "Client DLL: " << clientDll << std::endl;
			else			std::cout << "Fail :(" << std::endl;

			Sleep(3000);
		}

		while (!engineDll)
		{
			std::cout << "\nRequesting Engine DLL..." << std::endl;
			engineDll = g_MemoryManager.driver.GetEngineDll();
			if (engineDll)	std::cout << "Engine DLL: " << engineDll << std::endl;
			else			std::cout << "Fail :(" << std::endl;

			Sleep(3000);
		}

		bInit = true;
	}
}

void HackManager::ReadAddresses()
{
	localPlayer = g_MemoryManager.driver.ReadVirtualMemory<uintptr_t>(processId, clientDll + hazedumper::signatures::dwLocalPlayer, sizeof(uintptr_t));
	entityList = g_MemoryManager.driver.ReadVirtualMemory<uintptr_t>(processId, clientDll + hazedumper::signatures::dwEntityList, sizeof(uintptr_t));
	clientstate = g_MemoryManager.driver.ReadVirtualMemory<uintptr_t>(processId, engineDll + hazedumper::signatures::dwClientState, sizeof(uintptr_t));
}

void HackManager::ReadEntities()
{
	for (uint32_t i = 1; i < 64; i++)
	{
		entities[i] = NULL;
		uintptr_t entity = g_MemoryManager.driver.ReadVirtualMemory<uintptr_t>(processId, clientDll + hazedumper::signatures::dwEntityList + i * 0x10, sizeof(uintptr_t));
		if (entity) entities[i] = entity;
	}
}

void HackManager::NoFlash()
{
	g_MemoryManager.driver.WriteVirtualMemory<uint32_t>(processId, localPlayer + hazedumper::netvars::m_flFlashMaxAlpha, (uint32_t)0, sizeof(uint32_t));
}

void HackManager::Radar()
{
	for (int i = 1; i < 64; i++)
	{
		if (entities[i]) g_MemoryManager.driver.WriteVirtualMemory<bool>(processId, entities[i] + hazedumper::netvars::m_bSpotted, true, sizeof(bool));
	}
}

void HackManager::Aimbot()
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

void HackManager::Glow()
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
		uint32_t glowObject = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, clientDll + hazedumper::signatures::dwGlowObjectManager , sizeof(uint32_t));

		g_MemoryManager.driver.WriteVirtualMemory<float_t>(processId, glowObject + glowIndex * 0x38 + 0x8, 1-(hp/100), sizeof(float_t));
		g_MemoryManager.driver.WriteVirtualMemory<float_t>(processId, glowObject + glowIndex * 0x38 + 0xC, hp/100, sizeof(float_t));
		g_MemoryManager.driver.WriteVirtualMemory<float_t>(processId, glowObject + glowIndex * 0x38 + 0x10, 0.f, sizeof(float_t));
		g_MemoryManager.driver.WriteVirtualMemory<float_t>(processId, glowObject + glowIndex * 0x38 + 0x14, 1.f, sizeof(float_t));
		g_MemoryManager.driver.WriteVirtualMemory<bool>(processId, glowObject + glowIndex * 0x38 + 0x28, true, sizeof(bool));
		g_MemoryManager.driver.WriteVirtualMemory<bool>(processId, glowObject + glowIndex * 0x38 + 0x29, false, sizeof(bool));
	}
}

void HackManager::Trigger()
{
	uintptr_t pLocalplayer = g_MemoryManager.driver.ReadVirtualMemory<uintptr_t>(processId, clientDll + hazedumper::signatures::dwLocalPlayer, sizeof(uintptr_t));
	uint32_t localTeam = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, pLocalplayer + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));
	uint32_t inCrosshairId = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, pLocalplayer + hazedumper::netvars::m_iCrosshairId, sizeof(uint32_t));
	
	if (inCrosshairId > 0 && inCrosshairId < 64)
	{
		uintptr_t pPlayer = g_MemoryManager.driver.ReadVirtualMemory<uintptr_t>(processId, clientDll + hazedumper::signatures::dwEntityList + (inCrosshairId - 1) * 0x10, sizeof(uintptr_t));
		uint32_t inCrosshairTeam = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, pPlayer + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));
		int32_t inCrosshairHealth= g_MemoryManager.driver.ReadVirtualMemory<int32_t>(processId, pPlayer + hazedumper::netvars::m_iHealth, sizeof(int32_t));

		if (localTeam != inCrosshairTeam && inCrosshairHealth > 0 && (GetKeyState(VK_MENU) & 0x8000))
		{
			g_MemoryManager.driver.WriteVirtualMemory<uint32_t>(processId, clientDll + hazedumper::signatures::dwForceAttack, 1, sizeof(uint32_t));
			Sleep(1);
			g_MemoryManager.driver.WriteVirtualMemory<uint32_t>(processId, clientDll + hazedumper::signatures::dwForceAttack, 4, sizeof(uint32_t));
		}
	}
}

void HackManager::Bhop()
{
	int32_t flags = g_MemoryManager.driver.ReadVirtualMemory<int32_t>(processId, localPlayer + hazedumper::netvars::m_fFlags, sizeof(int32_t));
	uint32_t buffer;
	if (flags & 1) buffer = 6;
	else buffer = 4;

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		g_MemoryManager.driver.WriteVirtualMemory<uint32_t>(processId, clientDll + hazedumper::signatures::dwForceJump, buffer, sizeof(uint32_t));
}

void HackManager::Debug()
{
	for (int i = 1; i < 64; i++)
	{
		std::cout << "\nEntity " << i << " @ " << entities[i] << ":" << std::endl;
		int entityTeam = g_MemoryManager.driver.ReadVirtualMemory<uint32_t>(processId, entities[i] + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));
		std::cout << "Team: " << entityTeam << std::endl;
		bool alive = g_MemoryManager.driver.ReadVirtualMemory<int32_t>(processId, entities[i] + hazedumper::netvars::m_iHealth, sizeof(int32_t)) > 0 ? true : false;
		std::cout << "Alive: " << alive << std::endl;
	}
}
