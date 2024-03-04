#include "application/PluginManager.hpp"

#include <iostream>
#include <sstream>
#include <cassert>
#include <filesystem>

#include "application/PluginCore.hpp"
#include "application/Application.hpp"
#include "System.hpp"
#include "game/Game.hpp"
#include "game/GameObject.hpp"

PluginManager::PluginManager(Application* engine) :
	engine(engine)
{
}

PluginManager::~PluginManager()
{
	for (Plugin* p : plugins) delete p;
	plugins.clear();
}

void PluginManager::discoverAll(const std::filesystem::path& pluginsFolder)
{
	for (const std::filesystem::path& dllPath : engine->getSystem()->ListPlugins(pluginsFolder))
	{
		discover(dllPath);
	}
	std::cout << "Done discovering plugins\n";
}

Plugin* PluginManager::discover(const std::filesystem::path& dllPath)
{
	//Prevent double loads
	assert(std::find_if(plugins.begin(), plugins.end(), [&](Plugin* i) { return i->getPath() == dllPath; }) == plugins.end());
	
	Plugin* p = new Plugin(dllPath);
	std::cout << "Loading plugin: " << dllPath.filename() << '\n';
	p->load(engine);
	plugins.push_back(p);
	return p;
}

void PluginManager::load(Plugin* plugin)
{
	assert(std::find(plugins.begin(), plugins.end(), plugin) != plugins.end());
	plugin->load(engine);
}

void PluginManager::loadAll()
{
	for (Plugin* p : plugins) p->load(engine);
}

void PluginManager::unloadAll()
{
	for (Plugin* p : plugins) p->unload();
}

void PluginManager::hook(Plugin* plugin)
{
	assert(std::find(plugins.begin(), plugins.end(), plugin) != plugins.end());
	plugin->init();
}

void PluginManager::unhook(Plugin* plugin)
{
	assert(std::find(plugins.begin(), plugins.end(), plugin) != plugins.end());
	plugin->cleanup(false);
}

void PluginManager::hookAll()
{
	for (Plugin* p : plugins) p->init();
}

void PluginManager::unhookAll(bool shutdown)
{
	//TODO dependency tree
	for (Plugin* p : plugins) p->cleanup(shutdown);
}

void PluginManager::reloadAll()
{
	std::cout << "Hot Reload Started\n";

    std::cout << "Removing plugin hooks...\n";
    unhookAll(false);
    engine->getGame()->applyConcurrencyBuffers();

    std::cout << "Unloading plugin code...\n";
	unloadAll();

    std::cout << "Loading plugin code...\n";
	loadAll();

    std::cout << "Refreshing object layouts and vtables...\n";
	engine->getMemoryManager()->ensureFresh();

    std::cout << "Refreshing pointers... (call batchers)\n";
	engine->getGame()->refreshCallBatchers();

    std::cout << "Applying plugin hooks...\n";
    hookAll();
    
    std::cout << "Hot Reload Complete\n";
}

void PluginManager::enumeratePlugins(const std::function<void(Plugin*)>& visitor)
{
	for (Plugin* p : plugins) visitor(p);
}

Plugin const* PluginManager::getPlugin(const std::wstring& name)
{
	for (Plugin* p : plugins)
	{
		if (p->path == name) return p;
		if (p->reportedData && p->reportedData->name == name) return p;
	}
	return nullptr;
}
