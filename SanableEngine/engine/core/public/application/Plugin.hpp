#pragma once

#include "../dllapi.h"

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

#include "dllapi.h"
#include "PluginCore.hpp"

class ModuleTypeRegistry;
class Application;
class PluginManager;
class SerialFile;

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

	PluginReportedData* reportedData;
	SerialFile const* manifest; // Owned by PluginManager

	Plugin(const std::filesystem::path& pluginDir, const std::wstring& dllSubpath, SerialFile const* manifest);
	~Plugin();

	Plugin(const Plugin& cpy) = delete;
	Plugin(Plugin&& mov) noexcept;

	ENGINECORE_API void* getSymbol(const char* name) const;
	ENGINECORE_API std::filesystem::path getPluginDir() const;
	ENGINECORE_API std::filesystem::path getDllPath() const;
	ENGINECORE_API std::wstring getName() const;

	ENGINECORE_API bool isCodeLoaded() const;
	ENGINECORE_API bool isHooked() const;
	ENGINECORE_API const ModuleTypeRegistry* getRTTI() const;

	struct EntryPoints
	{
		fp_plugin_report      report      = nullptr;
		fp_plugin_init        init        = nullptr;
		fp_plugin_cleanup     cleanup     = nullptr;
		fp_plugin_reportTypes reportTypes = nullptr;
	};
private:
	friend class PluginManager;

	std::filesystem::path pluginDir;
	std::wstring dllSubpath;
	LibHandle dll;
	bool wasEverLoaded = false;
	bool wasEverHooked = false;

	EntryPoints entryPoints;

	bool load(Application const* context);
	bool init();
	bool cleanup(bool shutdown);
	void unload(Application* context);
};
