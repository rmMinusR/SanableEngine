#include "MemoryManager.hpp"

#include "GameObject.hpp"

void MemoryManager::init()
{
	//Pool instances are created by abusing static. Nothing to do here.
}

void MemoryManager::cleanup()
{
	SafeDisposable::disposeAll();
}
