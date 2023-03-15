#pragma once

#include "dllapi.h"

class MemoryManager;

class Asset
{
protected:
	bool isLoaded;

	virtual void loadInternal(MemoryManager*) = 0;
	virtual void unloadInternal(MemoryManager*) = 0;

public:
	ENGINEASSETS_API Asset();
	ENGINEASSETS_API virtual ~Asset();

	ENGINEASSETS_API void load(MemoryManager*);
	ENGINEASSETS_API void unload(MemoryManager*);
};
