#include "MemoryManager.hpp"

#include "GameObject.hpp"

void MemoryManager::destroy_wide(void* obj)
{
	for (auto it = SafeDisposable::all_begin(); it != SafeDisposable::all_end(); ++it)
	{
		RawMemoryPool* pool = dynamic_cast<RawMemoryPool*>(*it);
		if (pool && pool->contains(obj))
		{
			pool->freeSafe(obj);
			return;
		}
	}

	assert(false);
}

void MemoryManager::init()
{
	//Pool instances are created by abusing static. Nothing to do here.
}

void MemoryManager::cleanup()
{
	SafeDisposable::disposeAll();
}
