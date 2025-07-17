#pragma once

#include "../dllapi.h"

#include <vector>

#include "Plugin.hpp"

class ModuleTypeRegistry;
class Application;

class PluginManager
{
public:
	struct BufferedCommand
	{
		enum class Command
		{
			Load,
			Unload,
			Hook,
			Unhook
		} command;
		Plugin* plugin;
		bool cleanup_isShutdown;
		inline BufferedCommand(Command command, Plugin* plugin, bool cleanup_isShutdown = false) : command(command), plugin(plugin), cleanup_isShutdown(cleanup_isShutdown) {}
	};
private:
	Application* const engine;
	std::vector<Plugin*> plugins;
	std::vector<BufferedCommand> commandBuffer;
	
	//These execute immediately. USE WITH CAUTION.
	void loadAll();
	void unloadAll();
	void hookAll();
	void unhookAll(bool shutdown);

	void forgetAll();

	PluginManager(Application* engine);
	~PluginManager();

	friend class Application;

public:
	ENGINECORE_API size_t getNumPlugins() const;
	ENGINECORE_API void enumeratePlugins(const std::function<void(Plugin*)>& visitor);
	ENGINECORE_API Plugin const* getPlugin(const std::wstring& name);
	
	ENGINECORE_API Plugin* discover(const std::filesystem::path& pluginDir); // Note: lifetime managed by PluginManager

	//These go to the command buffer
	ENGINECORE_API void load(Plugin* plugin);
	ENGINECORE_API void unload(Plugin* plugin);
	ENGINECORE_API void hook(Plugin* plugin);
	ENGINECORE_API void unhook(Plugin* plugin);

	ENGINECORE_API size_t executeCommandBuffer(); // Not allowed almost anywhere for concurrency reasons
};
