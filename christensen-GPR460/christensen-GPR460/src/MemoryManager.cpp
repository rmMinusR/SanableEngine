#include "MemoryManager.hpp"

#include "GameObject.hpp"
#include "EngineCore.hpp"

void MemoryManager::destroy_wide(void* obj)
{
	if (!engine.quit) {

		for (int i = 0; i < allSafeDisposablesCount; ++i)
		{
			RawMemoryPool* pool = dynamic_cast<RawMemoryPool*>(allSafeDisposables[i]);
			if (pool && pool->contains(obj))
			{
				pool->freeSafe(obj);
				return;
			}
		}

		assert(false);
	}

}

void MemoryManager::init()
{
	//Pool instances are created by abusing static. Nothing to do here.
}

void MemoryManager::cleanup()
{
	SafeDisposable::disposeAll();
}
