#pragma once

#include "MemoryManager.hpp"
#include "Vector.hpp"

class Aimbot
{
public:
	Aimbot() {};

	void Run();

private:
	bool bAimer = true;
	bool bTrigger = true;

	void Aimer();
	void Trigger();

	Vector3 GetLocalPlayerHead();
	int GetEntityIdByCrosshair();
	Vector3 CalculateAngles(Vector3 source, Vector3 target);
	void NormalizeAngles(Vector3& angles);
};

