#pragma once

#include <vector>

#include "Plugin.h"

class EngineCore;

class PluginManager
{
private:
	std::vector<Plugin> plugins;
	
	void discoverAll(const std::wstring& pluginsFolderPath);
	void load(const std::wstring& dllPath);
	//void unloadAll();

	PluginManager();
	~PluginManager();

	friend class EngineCore;

public:
	//Plugin* getPlugin(const std::wstring& name);
};
