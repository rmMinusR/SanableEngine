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
#include "data/SerialFile.hpp"
#include "MemoryRoot.hpp"
#include "MemoryHeap.hpp"

PluginManager::PluginManager(Application* engine) :
	engine(engine)
{
}

PluginManager::~PluginManager()
{
	for (Plugin* p : plugins) delete p;
	plugins.clear();
}

Plugin* PluginManager::discover(const std::filesystem::path& pluginDir)
{
	// Build paths
	std::wostringstream dllFilename;
	dllFilename << pluginDir.filename().wstring() << PLATFORM_DLL_EXTENSION; // FIXME move to system
	std::filesystem::path manifestPath = pluginDir / "plugin.json";
	std::filesystem::path dllPath = pluginDir / dllFilename.str();

	// Check paths exist
	if (!std::filesystem::exists(manifestPath)) return nullptr; // Require manifest
	SerialFile* manifest = new SerialFile(manifestPath);

	// Prevent double loads
	for (Plugin* p : plugins)
	{
		if (p->getPluginDir() == pluginDir)
		{
			assert(false && "Plugin would be double loaded");
			return nullptr;
		}
	}
	
	// Init and register
	Plugin* p = new Plugin(pluginDir, dllFilename.str(), manifest);
	plugins.push_back(p);
	return p;
}

void PluginManager::unloadAll()
{
	for (Plugin* p : plugins) p->unload(engine);
}

void PluginManager::loadAll()
{
	for (Plugin* p : plugins) p->load(engine);
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

void PluginManager::forgetAll()
{
	for (Plugin* p : plugins)
	{
		delete p->manifest;
		delete p;
	}
	plugins.clear();
}

void PluginManager::unload(Plugin* plugin)
{
	assert(std::find(plugins.begin(), plugins.end(), plugin) != plugins.end());
	commandBuffer.emplace_back(BufferedCommand::Command::Unload, plugin);
}

void PluginManager::load(Plugin* plugin)
{
	assert(std::find(plugins.begin(), plugins.end(), plugin) != plugins.end());
	commandBuffer.emplace_back(BufferedCommand::Command::Load, plugin);
}

void PluginManager::hook(Plugin* plugin)
{
	assert(std::find(plugins.begin(), plugins.end(), plugin) != plugins.end());
	commandBuffer.emplace_back(BufferedCommand::Command::Hook, plugin);
}

void PluginManager::unhook(Plugin* plugin)
{
	assert(std::find(plugins.begin(), plugins.end(), plugin) != plugins.end());
	commandBuffer.emplace_back(BufferedCommand::Command::Unhook, plugin, false);
}

size_t PluginManager::executeCommandBuffer()
{
	for (const BufferedCommand& cmd : commandBuffer)
	{
		assert(std::find(plugins.begin(), plugins.end(), cmd.plugin) != plugins.end());
		switch (cmd.command)
		{
		case BufferedCommand::Command::Load  : cmd.plugin->load  (engine); break;
		case BufferedCommand::Command::Unload: cmd.plugin->unload(engine); break;
		case BufferedCommand::Command::Hook  : cmd.plugin->init(); break;
		case BufferedCommand::Command::Unhook: cmd.plugin->cleanup(cmd.cleanup_isShutdown); break;
		}
	}
	size_t numCommands = commandBuffer.size();
	commandBuffer.clear();
	return numCommands;
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
	MemoryRoot::get()->ensureFresh();

    std::cout << "Refreshing pointers... (call batchers)\n";
	engine->getGame()->refreshCallBatchers(true);

    std::cout << "Applying plugin hooks...\n";
    hookAll();
    
    std::cout << "Hot Reload Complete\n";
}

size_t PluginManager::getNumPlugins() const
{
	return plugins.size();
}

void PluginManager::enumeratePlugins(const std::function<void(Plugin*)>& visitor)
{
	for (Plugin* p : plugins) visitor(p);
}

Plugin const* PluginManager::getPlugin(const std::wstring& name)
{
	for (Plugin* p : plugins)
	{
		if (p->getName() == name) return p;
		if (p->reportedData && p->reportedData->name == name) return p;
	}
	return nullptr;
}
