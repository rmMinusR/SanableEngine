#include "AssetUser.hpp"

#include <cassert>

#include "Asset.hpp"

AssetUser::AssetUser()
{
	//Nothing to init
}

AssetUser::AssetUser(const std::vector<Asset*>& in)
{
	for (Asset* i : in)
	{
		resources.insert(i);
		i->requireBy(this);
	}
}

AssetUser::~AssetUser()
{
	for (Asset* i : resources)
	{
		i->releaseBy(this);
	}
	resources.clear();
}

void AssetUser::require(Asset* res)
{
	assert(resources.find(res) == resources.end());
	resources.insert(res);
	res->requireBy(this);
}

void AssetUser::release(Asset* res)
{
	assert(resources.find(res) != resources.end());
	resources.erase(res);
	res->releaseBy(this);
}
