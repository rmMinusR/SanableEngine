#include "PluginManager.hpp"

#include <sstream>
#include <cassert>
#include <filesystem>

#include "EngineCore.hpp"

PluginManager::PluginManager()
{
}

PluginManager::~PluginManager()
{
	assert(plugins.size() == 0);
}

void PluginManager::discoverAll(const std::filesystem::path& pluginsFolder, EngineCore* engine)
{
	std::ostringstream joiner;

	//Discover, create a wrapper and load the DLL of each
	std::vector<Plugin*> batch;
	for (const std::filesystem::path& dllPath : engine->getSystem()->ListPlugins(pluginsFolder))
	{
		Plugin* p = &plugins.emplace_back(dllPath);
		p->loadDLL();
		batch.push_back(p); //Defer plugin_preInit call
	}

	for (Plugin* p : batch) p->preInit(engine);

	//TODO dependency tree
	for (Plugin* p : batch) p->init();
}

void PluginManager::load(const std::wstring& dllPath, EngineCore* engine)
{
	Plugin* p = &plugins.emplace_back(dllPath);
	p->loadDLL();
	p->preInit(engine);
}

void PluginManager::unloadAll()
{
	//TODO dependency tree
	for (Plugin& p : plugins) p.cleanup();

	for (Plugin& p : plugins) p.unloadDLL();
	plugins.clear();
}
