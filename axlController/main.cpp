#include "MemoryManager.hpp"
#include "HackManager.hpp"

int main()
{
	g_MemoryManager = MemoryManager();

	HackManager hm = HackManager();

	hm.Run();
}
