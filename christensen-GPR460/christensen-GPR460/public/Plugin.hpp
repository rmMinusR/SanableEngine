#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
typedef HMODULE LibHandle;
#define InvalidLibHandle ((HMODULE)INVALID_HANDLE_VALUE)
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
typedef void* LibHandle;
#define InvalidLibHandle (nullptr)
#endif

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

	//char const* name;

	//unsigned int versionID;
	//char const* versionString;

	//char const* const* dependencies;
	//unsigned int dependencyCount;

	__thiscall Plugin(const std::filesystem::path& path);
	~Plugin();

	Plugin(const Plugin& cpy) = delete;
	__thiscall Plugin(Plugin&& mov) noexcept;

	void* getSymbol(const char* name) const;

private:
	friend class PluginManager;

	std::filesystem::path path;
	LibHandle dll;

	bool _dllGood() const;

	void loadDLL();
	void preInit(EngineCore* engine);
	void init();
	void cleanup();
	void unloadDLL();
};