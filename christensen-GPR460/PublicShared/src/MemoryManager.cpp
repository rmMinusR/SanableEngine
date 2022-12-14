#include "MemoryManager.hpp"

#include "GameObject.hpp"

void MemoryManager::init()
{
	//Nothing to do here
}

void MemoryManager::cleanup()
{
	for (RawMemoryPool* p : pools) if (p) delete p;
	pools.clear();
}
