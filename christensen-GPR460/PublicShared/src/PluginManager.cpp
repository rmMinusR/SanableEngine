#include "PluginManager.hpp"

#include <sstream>
#include <cassert>
#include <filesystem>

#include "EngineCore.hpp"
#include "System.hpp"

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

void PluginManager::unloadAll()
{
	//TODO dependency tree
	for (Plugin* p : plugins) p->cleanup();

	for (Plugin* p : plugins) p->unloadDLL();
	for (Plugin* p : plugins) delete p;
	plugins.clear();
}
