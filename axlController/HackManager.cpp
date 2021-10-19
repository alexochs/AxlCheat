#include "HackManager.hpp"

HackManager::HackManager()
{
	Init();
	ReadAddresses();
	misc = Misc();
	intel = Intel();
	aimbot = Aimbot();
}

void HackManager::Run()
{
	while (true)
	{
		ReadAddresses();
		ReadEntities();

		misc.Run();
		intel.Run();
		aimbot.Run();

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

		std::cout << "\n\n=============================\nSuccessfully initialized! :)\n=============================";
		std::cout << "\n\n\n(C) Copyright 2021 alexochs.de";
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
