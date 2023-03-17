#pragma once

#include "dllapi.h"
#include <unordered_set>

class MemoryManager;
class AssetUser;

class Asset
{
public:
	enum class LoadPolicy
	{
		Lazy, 
		Persistent,

		Default = Lazy
	};

protected:
	bool isLoaded;
	LoadPolicy loadPolicy;

	virtual void loadInternal(MemoryManager*) = 0;
	virtual void unloadInternal(MemoryManager*) = 0;

	void updateIsLoaded();

	friend class AssetUser;
	void requireBy(AssetUser*);
	void releaseBy(AssetUser*);
	std::unordered_set<AssetUser*> usedBy;
public:
	ENGINEASSETS_API Asset();
	ENGINEASSETS_API virtual ~Asset();

	ENGINEASSETS_API void setLoadPolicy(LoadPolicy loadPolicy);
};
