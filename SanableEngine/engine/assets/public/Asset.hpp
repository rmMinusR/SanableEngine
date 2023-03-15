#pragma once

class MemoryManager;

class Asset
{
protected:
	bool isLoaded;

	virtual void loadInternal(MemoryManager*) = 0;
	virtual void unloadInternal(MemoryManager*) = 0;

public:
	Asset();
	virtual ~Asset();

	void load(MemoryManager*);
	void unload(MemoryManager*);
};
