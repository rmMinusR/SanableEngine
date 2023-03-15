#include "Asset.hpp"

#include <cassert>

Asset::Asset()
{
	isLoaded = false;
}

Asset::~Asset()
{
	assert(!isLoaded);
}

void Asset::load(MemoryManager* memMgr)
{
	assert(!isLoaded);
	loadInternal(memMgr);
}

void Asset::unload(MemoryManager* memMgr)
{
	assert(isLoaded);
	unloadInternal(memMgr);
}
