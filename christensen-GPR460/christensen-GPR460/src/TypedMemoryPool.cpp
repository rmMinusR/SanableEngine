#include "TypedMemoryPool.hpp"

std::vector<SafeDisposable*> allSafeDisposables = std::vector<SafeDisposable*>(8);

void SafeDisposable::disposeAll()
{
	for (SafeDisposable* f : allSafeDisposables) f->disposeContents();
}

SafeDisposable::SafeDisposable()
{
	allSafeDisposables.push_back(this);
}

SafeDisposable::~SafeDisposable()
{
	if (allSafeDisposables.size() != 0) //Awful special case for atexit cleanup
	{
		auto it = std::remove(allSafeDisposables.begin(), allSafeDisposables.end(), this);
		allSafeDisposables.erase(it);
	}
}

std::vector<SafeDisposable*>::iterator SafeDisposable::all_begin()
{
	return allSafeDisposables.begin();
}

std::vector<SafeDisposable*>::iterator SafeDisposable::all_end()
{
	return allSafeDisposables.end();
}
