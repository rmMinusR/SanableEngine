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
		batch.push_back(p); //Defer registerContents call
	}

	//Load contents of each
	for (Plugin* p : batch) p->registerContents();
}

void PluginManager::load(const std::wstring& dllPath)
{
	Plugin* p = &plugins.emplace_back(dllPath);
	p->loadDLL();
	p->registerContents();
}
