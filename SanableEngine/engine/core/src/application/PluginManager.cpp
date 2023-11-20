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
	assert(plugins.size() == 0);
}

void PluginManager::discoverAll(const std::filesystem::path& pluginsFolder)
{
	//Discover, create a wrapper and load the DLL of each
	std::vector<Plugin*> batch;
	for (const std::filesystem::path& dllPath : engine->getSystem()->ListPlugins(pluginsFolder))
	{
		Plugin* p = new Plugin(dllPath);
		std::cout << "Importing plugin code: " << dllPath.filename() << '\n';
		p->loadDLL();
		batch.push_back(p); //Defer plugin_preInit call
		plugins.push_back(p);
	}

	for (Plugin* p : batch)
	{
		std::cout << "Registering plugin: " << p->path.filename() << '\n';
		p->preInit(engine);
		std::cout << "Reported name: " << p->reportedData->name << '\n';
	}

	for (Plugin* p : batch)
	{
		p->tryRegisterTypes();
	}

	for (Plugin* p : batch)
	{
		std::cout << "Applying plugin hooks for " << p->reportedData->name << '\n';
		p->init(true);
	}

	std::cout << "Done discovering plugins\n";
}

void PluginManager::load(const std::wstring& dllPath)
{
	Plugin* p = new Plugin(dllPath);
	p->loadDLL();
	p->preInit(engine);
	p->tryRegisterTypes();
	plugins.push_back(p);
}

void PluginManager::hookAll(bool firstRun)
{
	for (Plugin* p : plugins) p->init(firstRun);
}

void PluginManager::unhookAll(bool shutdown)
{
	//TODO dependency tree
	for (Plugin* p : plugins) p->cleanup(shutdown);
}

void PluginManager::unloadAll()
{
	for (Plugin* p : plugins) p->unloadDLL();
	for (Plugin* p : plugins) delete p;
	plugins.clear();
}

void PluginManager::reloadAll()
{
	std::cout << "Hot Reload Started\n";

    std::cout << "Removing plugin hooks...\n";
    unhookAll(false);
    engine->getGame()->applyConcurrencyBuffers();

    std::cout << "Unloading plugin code...\n";
	for (Plugin* p : plugins) p->unloadDLL();

    std::cout << "Loading plugin code...\n";
	for (Plugin* p : plugins) p->loadDLL();
	for (Plugin* p : plugins) p->preInit(engine);

    std::cout << "Refreshing object layouts and vtables...\n";
	for (Plugin* p : plugins) p->tryRegisterTypes();
	engine->getMemoryManager()->ensureFresh();

    std::cout << "Applying plugin hooks...\n";
    hookAll(false);
    
    std::cout << "Refreshing pointers... (call batchers)\n";
	engine->getGame()->refreshCallBatchers();

    std::cout << "Hot Reload Complete\n";
}