#pragma once

#include "../dllapi.h"

#include <vector>

#include "Plugin.hpp"

class ModuleTypeRegistry;
class Application;

class PluginManager
{
private:
	Application* const engine;
	std::vector<Plugin*> plugins;
	
	void discoverAll(const std::filesystem::path& pluginsFolder);
	void load(const std::wstring& dllPath);
	void hookAll(bool firstRun);
	void unhookAll(bool shutdown);
	void unloadAll();

	void reloadAll();

	PluginManager(Application* engine);
	~PluginManager();

	friend class Application;

public:
	//Plugin const* getPlugin(const std::wstring& name);
};
