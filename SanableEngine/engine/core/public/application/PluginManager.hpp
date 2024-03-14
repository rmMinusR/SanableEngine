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

	void loadAll();
	void unloadAll();

	void hookAll();
	void unhookAll(bool shutdown);

	void reloadAll();

	PluginManager(Application* engine);
	~PluginManager();

	friend class Application;

public:
	ENGINECORE_API void enumeratePlugins(const std::function<void(Plugin*)>& visitor);
	ENGINECORE_API Plugin const* getPlugin(const std::wstring& name);
	
	ENGINECORE_API Plugin* discover(const std::filesystem::path& dllPath);

	ENGINECORE_API void load(Plugin* plugin);
	ENGINECORE_API void unload(Plugin* plugin);

	ENGINECORE_API void hook(Plugin* plugin);
	ENGINECORE_API void unhook(Plugin* plugin);
};
