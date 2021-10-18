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
	std::cout << "Initializing...\n" << std::endl;


	std::cout << "Loading Driver..." << std::endl;
	driver = KernelInterface("\\\\.\\axlkmd");

	/*std::cout << "Setting Module: ac_client.exe" << std::endl;
	if (Driver.SetModulePathSubstring(L"\\AssaultCube\\bin_win32\\ac_client.exe lol"))
		std::cout << "Successfully initialized! \n" << std::endl;
	else
		std::cout << "Fail :( \n" << std::endl;*/;

	std::cout << "\nRequesting Process ID..." << std::endl;
	processId = driver.GetProcessId();
	if (processId)	std::cout << "Process ID: " << processId << std::endl;
	else			std::cout << "Fail :(" << std::endl;

	std::cout << "\nRequesting Client DLL..." << std::endl;
	clientDll = driver.GetClientDll();
	if (clientDll)	std::cout << "Client DLL: " << clientDll << std::endl;
	else			std::cout << "Fail :(" << std::endl;

	std::cout << "\nRequesting Engine DLL..." << std::endl;
	engineDll = driver.GetEngineDll();
	if (engineDll)	std::cout << "Engine DLL: " << engineDll << std::endl;
	else			std::cout << "Fail :(" << std::endl;

	bool bInit = false;
	while (!bInit)
	{
		while (!processId)
		{
			std::cout << "\nRequesting Process ID..." << std::endl;

			processId = driver.GetProcessId();
			if (processId)	std::cout << "Process ID: " << processId << std::endl;
			else			std::cout << "Fail :(\nTrying again in 3 seconds..." << std::endl;

			Sleep(3000);
		}

		while (!clientDll)
		{
			std::cout << "\nRequesting Client DLL..." << std::endl;
			clientDll = driver.GetClientDll();
			if (clientDll)	std::cout << "Client DLL: " << clientDll << std::endl;
			else			std::cout << "Fail :(" << std::endl;

			Sleep(3000);
		}

		while (!engineDll)
		{
			std::cout << "\nRequesting Engine DLL..." << std::endl;
			engineDll = driver.GetEngineDll();
			if (engineDll)	std::cout << "Engine DLL: " << engineDll << std::endl;
			else			std::cout << "Fail :(" << std::endl;

			Sleep(3000);
		}

		bInit = true;
	}
}

void HackManager::ReadAddresses()
{
	localPlayer = driver.ReadVirtualMemory<uintptr_t>(processId, clientDll + hazedumper::signatures::dwLocalPlayer, sizeof(uintptr_t));
	entityList = driver.ReadVirtualMemory<uintptr_t>(processId, clientDll + hazedumper::signatures::dwEntityList, sizeof(uintptr_t));
	clientstate = driver.ReadVirtualMemory<uintptr_t>(processId, engineDll + hazedumper::signatures::dwClientState, sizeof(uintptr_t));
}

void HackManager::ReadEntities()
{
	for (uint32_t i = 1; i < 64; i++)
	{
		entities[i] = NULL;
		uintptr_t entity = driver.ReadVirtualMemory<uintptr_t>(processId, clientDll + hazedumper::signatures::dwEntityList + i * 0x10, sizeof(uintptr_t));
		if (entity) entities[i] = entity;
	}
}

void HackManager::NoFlash()
{
	driver.WriteVirtualMemory<uint32_t>(processId, localPlayer + hazedumper::netvars::m_flFlashMaxAlpha, (uint32_t)0, sizeof(uint32_t));
}

void HackManager::Radar()
{
	for (int i = 1; i < 64; i++)
	{
		if (entities[i]) driver.WriteVirtualMemory<bool>(processId, entities[i] + hazedumper::netvars::m_bSpotted, true, sizeof(bool));
	}
}

void HackManager::Aimbot()
{
	float_t localoriginX = driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecOrigin, sizeof(float_t));
	float_t localoriginY = driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecOrigin + sizeof(float_t), sizeof(float_t));
	float_t localoriginZ = driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecOrigin + 2 * sizeof(float_t), sizeof(float_t));
	float_t localoffsetX = driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecViewOffset, sizeof(float_t));
	float_t localoffsetY = driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecViewOffset + sizeof(float_t), sizeof(float_t));
	float_t localoffsetZ = driver.ReadVirtualMemory<float_t>(processId, localPlayer + hazedumper::netvars::m_vecViewOffset + 2 * sizeof(float_t), sizeof(float_t));
	Vector3 local = { localoriginX + localoffsetX, localoriginY + localoffsetY, localoriginZ + localoffsetZ };
	uint32_t localTeam = driver.ReadVirtualMemory<uint32_t>(processId, localPlayer + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));

	float aimPitch, aimYaw;

	for (int i = 0; i < 64; i++)
	{
		if (!entities[i])
			continue;

		int32_t hp = driver.ReadVirtualMemory<uint32_t>(processId, entities[i] + hazedumper::netvars::m_iHealth, sizeof(int32_t));
		if (hp < 1)
			continue;

		uint32_t entityTeam = driver.ReadVirtualMemory<uint32_t>(processId, entities[i] + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));
		if (localTeam == entityTeam)
			continue;

		float_t originX = driver.ReadVirtualMemory<float_t>(processId, entities[i] + hazedumper::netvars::m_vecOrigin, sizeof(float_t));
		float_t originY = driver.ReadVirtualMemory<float_t>(processId, entities[i] + hazedumper::netvars::m_vecOrigin + sizeof(float_t), sizeof(float_t));
		float_t originZ = driver.ReadVirtualMemory<float_t>(processId, entities[i] + hazedumper::netvars::m_vecOrigin + 2*sizeof(float_t), sizeof(float_t));
		float_t offsetX = driver.ReadVirtualMemory<float_t>(processId, entities[i] + hazedumper::netvars::m_vecViewOffset, sizeof(float_t));
		float_t offsetY = driver.ReadVirtualMemory<float_t>(processId, entities[i] + hazedumper::netvars::m_vecViewOffset + sizeof(float_t), sizeof(float_t));
		float_t offsetZ = driver.ReadVirtualMemory<float_t>(processId, entities[i] + hazedumper::netvars::m_vecViewOffset + 2 * sizeof(float_t), sizeof(float_t));
		Vector3 head = { originX + offsetX, originY + offsetY, originZ + offsetZ };
	
		Vector3 dst = head - local;

		float yaw = atan(dst.x / dst.y) * 180 / 3.14;
		float pitch = -atan(dst.z / sqrt(dst.x * dst.x + dst.y * dst.y)) * 180 / 3.14;
	}

	driver.WriteVirtualMemory<float_t>(processId, clientstate + hazedumper::signatures::dwClientState_ViewAngles, aimPitch, sizeof(float_t));
	driver.WriteVirtualMemory<float_t>(processId, clientstate + hazedumper::signatures::dwClientState_ViewAngles + sizeof(float_t), aimYaw, sizeof(float_t));
}

void HackManager::Glow()
{
	uint32_t localTeam = driver.ReadVirtualMemory<uint32_t>(processId, localPlayer + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));

	for (int i = 1; i < 64; i++)
	{
		uint32_t pPlayer = driver.ReadVirtualMemory<uint32_t>(processId, clientDll + hazedumper::signatures::dwEntityList + i * 0x10, sizeof(uint32_t));

		if (!pPlayer)
			continue;
		
		int32_t hp = driver.ReadVirtualMemory<uint32_t>(processId, pPlayer + hazedumper::netvars::m_iHealth, sizeof(int32_t));
		if (hp < 1)
			continue;

		uint32_t entityTeam = driver.ReadVirtualMemory<uint32_t>(processId, pPlayer + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));
		if (localTeam == entityTeam)
			continue;

		uint32_t glowIndex = driver.ReadVirtualMemory<uint32_t>(processId, pPlayer + hazedumper::netvars::m_iGlowIndex, sizeof(uint32_t));
		uint32_t glowObject = driver.ReadVirtualMemory<uint32_t>(processId, clientDll + hazedumper::signatures::dwGlowObjectManager , sizeof(uint32_t));

		driver.WriteVirtualMemory<float_t>(processId, glowObject + glowIndex * 0x38 + 0x8, 1.f, sizeof(float_t));
		driver.WriteVirtualMemory<float_t>(processId, glowObject + glowIndex * 0x38 + 0xC, 0.f, sizeof(float_t));
		driver.WriteVirtualMemory<float_t>(processId, glowObject + glowIndex * 0x38 + 0x10, 0.f, sizeof(float_t));
		driver.WriteVirtualMemory<float_t>(processId, glowObject + glowIndex * 0x38 + 0x14, 1.f, sizeof(float_t));
		driver.WriteVirtualMemory<bool>(processId, glowObject + glowIndex * 0x38 + 0x28, true, sizeof(bool));
		driver.WriteVirtualMemory<bool>(processId, glowObject + glowIndex * 0x38 + 0x29, false, sizeof(bool));
	}
}

void HackManager::Trigger()
{
	uintptr_t pLocalplayer = driver.ReadVirtualMemory<uintptr_t>(processId, clientDll + hazedumper::signatures::dwLocalPlayer, sizeof(uintptr_t));
	uint32_t localTeam = driver.ReadVirtualMemory<uint32_t>(processId, pLocalplayer + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));
	uint32_t inCrosshairId = driver.ReadVirtualMemory<uint32_t>(processId, pLocalplayer + hazedumper::netvars::m_iCrosshairId, sizeof(uint32_t));
	
	if (inCrosshairId > 0 && inCrosshairId < 64)
	{
		uintptr_t pPlayer = driver.ReadVirtualMemory<uintptr_t>(processId, clientDll + hazedumper::signatures::dwEntityList + (inCrosshairId - 1) * 0x10, sizeof(uintptr_t));
		uint32_t inCrosshairTeam = driver.ReadVirtualMemory<uint32_t>(processId, pPlayer + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));
		int32_t inCrosshairHealth= driver.ReadVirtualMemory<int32_t>(processId, pPlayer + hazedumper::netvars::m_iHealth, sizeof(int32_t));

		if (localTeam != inCrosshairTeam && inCrosshairHealth > 0 && (GetKeyState(VK_MENU) & 0x8000))
		{
			driver.WriteVirtualMemory<uint32_t>(processId, clientDll + hazedumper::signatures::dwForceAttack, 1, sizeof(uint32_t));
			Sleep(1);
			driver.WriteVirtualMemory<uint32_t>(processId, clientDll + hazedumper::signatures::dwForceAttack, 4, sizeof(uint32_t));
		}
	}
}

void HackManager::Bhop()
{
	int32_t flags = driver.ReadVirtualMemory<int32_t>(processId, localPlayer + hazedumper::netvars::m_fFlags, sizeof(int32_t));
	uint32_t buffer;
	if (flags & 1) buffer = 6;
	else buffer = 4;

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		driver.WriteVirtualMemory<uint32_t>(processId, clientDll + hazedumper::signatures::dwForceJump, buffer, sizeof(uint32_t));
}

void HackManager::Debug()
{
	for (int i = 1; i < 64; i++)
	{
		std::cout << "\nEntity " << i << " @ " << entities[i] << ":" << std::endl;
		int entityTeam = driver.ReadVirtualMemory<uint32_t>(processId, entities[i] + hazedumper::netvars::m_iTeamNum, sizeof(uint32_t));
		std::cout << "Team: " << entityTeam << std::endl;
		bool alive = driver.ReadVirtualMemory<int32_t>(processId, entities[i] + hazedumper::netvars::m_iHealth, sizeof(int32_t)) > 0 ? true : false;
		std::cout << "Alive: " << alive << std::endl;
	}
}
