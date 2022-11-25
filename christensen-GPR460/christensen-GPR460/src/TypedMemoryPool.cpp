#include "TypedMemoryPool.hpp"

std::vector<SafeDisposable*> SafeDisposable::all;

void SafeDisposable::disposeAll()
{
	for (SafeDisposable* f : all) f->disposeContents();
}

SafeDisposable::SafeDisposable()
{
	SafeDisposable::all.push_back(this);
}

SafeDisposable::~SafeDisposable()
{
	if (SafeDisposable::all.size() != 0) //Awful special case for atexit cleanup
	{
		auto it = std::remove(SafeDisposable::all.begin(), SafeDisposable::all.end(), this);
		SafeDisposable::all.erase(it);
	}
}