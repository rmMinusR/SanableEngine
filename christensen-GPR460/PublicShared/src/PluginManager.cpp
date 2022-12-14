#include "PluginManager.hpp"

#include <iostream>
#include <sstream>
#include <cassert>
#include <filesystem>

#include "EngineCore.hpp"
#include "System.hpp"
#include "PluginCore.hpp"
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
		p->init();
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

void PluginManager::hookAll()
{
	for (Plugin* p : plugins) p->init();
}

void PluginManager::unhookAll()
{
	//TODO dependency tree
	for (Plugin* p : plugins) p->cleanup();
}

void PluginManager::unloadAll()
{
	for (Plugin* p : plugins) p->unloadDLL();
	for (Plugin* p : plugins) delete p;
	plugins.clear();
}

void PluginManager::refreshVtablePointers()
{
	//Build new list of vtable pointers
	std::vector<HotswapTypeData*> refreshers;
	for (Plugin* p : plugins) for (HotswapTypeData& d : p->reportedData->hotswappables) refreshers.push_back(&d);

	engine->getMemoryManager()->refreshVtables(refreshers);

	//Write to objects
	for (auto go_it = engine->objects_cbegin(); go_it != engine->objects_cend(); ++go_it)
	{
		GameObject* go = *go_it;
		go->refreshVtables(refreshers);
	}
}
