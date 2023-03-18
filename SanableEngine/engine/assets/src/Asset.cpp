#include "Asset.hpp"

#include <cassert>

#include "AssetUser.hpp"

Asset::Asset() :
	isLoaded(false),
	loadPolicy(LoadPolicy::Default)
{
}

Asset::~Asset()
{
	assert(!isLoaded);
}

void Asset::updateIsLoaded()
{
	bool shouldLoad = usedBy.size() != 0 || loadPolicy == LoadPolicy::Persistent;

	if ( shouldLoad && !isLoaded)   loadInternal();
	if (!shouldLoad &&  isLoaded) unloadInternal();
}

void Asset::requireBy(AssetUser* x)
{
	usedBy.insert(x);
	updateIsLoaded();
}

void Asset::releaseBy(AssetUser* x)
{
	usedBy.erase(x);
	updateIsLoaded();
}


void Asset::setLoadPolicy(LoadPolicy loadPolicy)
{
	this->loadPolicy = loadPolicy;
	updateIsLoaded();
}