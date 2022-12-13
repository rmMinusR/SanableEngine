#pragma once

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
	void unloadAll();

	PluginManager(EngineCore* engine);
	~PluginManager();

	friend class EngineCore;

public:
	//Plugin const* getPlugin(const std::wstring& name);
};
