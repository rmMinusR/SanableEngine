#include "PluginManager.hpp"

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
		p->loadDLL();
		batch.push_back(p); //Defer plugin_preInit call
		plugins.push_back(p);
	}

	for (Plugin* p : batch) p->preInit(engine);

	//TODO dependency tree
	for (Plugin* p : batch) p->init();
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
	std::vector<HotswapTypeData*> refreshers;
	for (Plugin* p : plugins) for (HotswapTypeData& d : p->reportedData->hotswappables) refreshers.push_back(&d);

	for (auto go_it = engine->objects_cbegin(); go_it != engine->objects_cend(); ++go_it)
	{
		GameObject* go = *go_it;
		go->hotswapRefresh(refreshers);
	}
}
