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
	isLoaded = true;
	loadInternal(memMgr);
}

void Asset::unload(MemoryManager* memMgr)
{
	assert(isLoaded);
	isLoaded = false;
	unloadInternal(memMgr);
}
