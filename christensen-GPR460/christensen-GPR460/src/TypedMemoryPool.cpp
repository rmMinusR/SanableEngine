#include "TypedMemoryPool.hpp"

std::vector<SafeDisposable*> SafeDisposable::all;

SafeDisposable::SafeDisposable()
{
	SafeDisposable::all.push_back(this);
}

SafeDisposable::~SafeDisposable()
{
	SafeDisposable::all.erase(std::find(SafeDisposable::all.cbegin(), SafeDisposable::all.cend(), this));
}