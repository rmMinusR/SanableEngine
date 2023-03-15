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

void Asset::load()
{
	assert(!isLoaded);
	loadInternal();
}

void Asset::unload()
{
	assert(isLoaded);
	unloadInternal();
}
