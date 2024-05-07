#pragma once

#include "../dllapi.h"

#include <vector>

#include "Plugin.hpp"
#include "ThunkUtils.hpp"

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
	
	void discoverAll(const std::filesystem::path& pluginsFolder);

	//These execute immediately. USE WITH CAUTION.
	void loadAll();
	void unloadAll();
	void hookAll();
	void unhookAll(bool shutdown);

	void forgetAll();

	size_t executeCommandBuffer();

	void reloadAll(); //TODO refactor into Application

	PluginManager(Application* engine);
	~PluginManager();

	friend class Application;
	friend struct thunk_utils<PluginManager>;

public:
	ENGINECORE_API void enumeratePlugins(const std::function<void(Plugin*)>& visitor);
	ENGINECORE_API Plugin const* getPlugin(const std::wstring& name);
	
	ENGINECORE_API Plugin* discover(const std::filesystem::path& dllPath);

	//These go to the command buffer
	ENGINECORE_API void load(Plugin* plugin);
	ENGINECORE_API void unload(Plugin* plugin);
	ENGINECORE_API void hook(Plugin* plugin);
	ENGINECORE_API void unhook(Plugin* plugin);
};
