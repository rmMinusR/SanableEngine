#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <functional>
#include <filesystem>
#include <vector>
#include <string>

class EngineCore;
class PluginManager;

struct Plugin
{
public:
	enum class Status
	{
		NotLoaded = 0,

		DllLoaded, //DLL is loaded but no functions have been called
		Registered, //Plugin has been registered with the dependency tree
		Hooked, //Plugin has registered to hooks and callbacks

		LoadComplete = Hooked
	} status;

	HMODULE dll;

	//char const* name;

	//unsigned int versionID;
	//char const* versionString;

	//char const* const* dependencies;
	//unsigned int dependencyCount;

	Plugin(const std::filesystem::path& path);
	~Plugin();

private:
	friend class PluginManager;

	std::filesystem::path path;

	inline bool _dllGood() { return dll != INVALID_HANDLE_VALUE; }

	void loadDLL();
	void plugin_preInit(EngineCore* engine);
	void init();
	void plugin_cleanup();
	void unloadDLL();
};
