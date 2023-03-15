#pragma once

class Asset
{
protected:
	bool isLoaded;

	virtual void loadInternal() = 0;
	virtual void unloadInternal() = 0;

public:
	Asset();
	virtual ~Asset();

	void load();
	void unload();
};
