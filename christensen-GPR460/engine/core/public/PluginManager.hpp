#pragma once

#include "dllapi.h"

#include <vector>

#include "Plugin.hpp"

class EngineCore;

class PluginManager
{
private:
	EngineCore* const engine;
	std::vector<Plugin*> plugins;
	
	void discoverAll(const std::filesystem::path& pluginsFolder);
	void load(const std::wstring& dllPath);
	void hookAll(bool firstRun);
	void unhookAll(bool shutdown);
	void unloadAll();

	void reloadAll();

	void refreshVtablePointers();

	PluginManager(EngineCore* engine);
	~PluginManager();

	friend class EngineCore;

public:
	//Plugin const* getPlugin(const std::wstring& name);
};
