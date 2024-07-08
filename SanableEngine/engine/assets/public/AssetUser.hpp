#pragma once

#include <unordered_set>

#include "dllapi.h"

class Asset;
class MemoryHeap;

class AssetUser
{
private:
	std::unordered_set<Asset*> resources;

protected:
	ENGINEASSETS_API AssetUser();
	ENGINEASSETS_API AssetUser(const std::vector<Asset*>&);
	ENGINEASSETS_API virtual ~AssetUser();

	ENGINEASSETS_API void require(Asset*);
	ENGINEASSETS_API void release(Asset*);

	//Protect against accidental copying/moving
	AssetUser(const AssetUser& cpy) = delete;
	AssetUser(AssetUser&& mov) = delete;
	AssetUser& operator=(const AssetUser& cpy) = delete;
	AssetUser& operator=(AssetUser&& mov) = delete;
};
