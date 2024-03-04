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
	Plugin* discover(const std::filesystem::path& dllPath);

	void load(Plugin* plugin);
	void unload(Plugin* plugin);
	void loadAll();
	void unloadAll();

	void hookAll();
	void unhookAll(bool shutdown);

	void reloadAll();

	PluginManager(Application* engine);
	~PluginManager();

	friend class Application;

public:
	//Plugin const* getPlugin(const std::wstring& name);
};
