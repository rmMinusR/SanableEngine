#include "PluginManager.hpp"

#include <iostream>
#include <sstream>
#include <cassert>
#include <filesystem>

#include "PluginCore.hpp"
#include "EngineCore.hpp"
#include "System.hpp"
#include "GameObject.hpp"

PluginManager::PluginManager(EngineCore* engine) :
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
		std::cout << "Applying plugin hooks for " << p->reportedData->name << '\n';
		p->init(true, engine);
	}

	std::cout << "Done discovering plugins\n";
}

void PluginManager::load(const std::wstring& dllPath)
{
	Plugin* p = new Plugin(dllPath);
	p->loadDLL();
	p->preInit(engine);
	plugins.push_back(p);
}

void PluginManager::hookAll(bool firstRun)
{
	for (Plugin* p : plugins) p->init(firstRun, engine);
}

void PluginManager::unhookAll(bool shutdown)
{
	//TODO dependency tree
	for (Plugin* p : plugins) p->cleanup(shutdown, engine);
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
    engine->applyConcurrencyBuffers();

    std::cout << "Unloading plugin code...\n";
	for (Plugin* p : plugins) p->unloadDLL();

    std::cout << "Loading plugin code...\n";
	for (Plugin* p : plugins) p->loadDLL();
	for (Plugin* p : plugins) p->preInit(engine);

    std::cout << "Refreshing pointers... (vtables)\n";
    refreshVtablePointers();

    std::cout << "Applying plugin hooks...\n";
    hookAll(false);
    
    std::cout << "Refreshing pointers... (call batchers)\n";
    engine->refreshCallBatchers();

    std::cout << "Hot Reload Complete\n";
}

void PluginManager::refreshVtablePointers()
{
	//Build new list of vtable pointers
	std::vector<TypeInfo*> refreshers;
	for (Plugin* p : plugins) for (TypeInfo& d : p->reportedData->rtti.types) refreshers.push_back(&d);

	engine->getMemoryManager()->refreshVtables(refreshers);
}
