#include "Asset.hpp"

#include <cassert>

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

	if ( shouldLoad && !isLoaded)   loadInternal(nullptr);
	if (!shouldLoad &&  isLoaded) unloadInternal(nullptr);
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