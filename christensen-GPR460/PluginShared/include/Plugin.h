#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <functional>
#include <filesystem>
#include <vector>
#include <string>

class PluginManager;

struct Plugin
{
public:
	enum class Status
	{
		NotLoaded = 0,

		DllLoaded,
		Registered,
		ContentLoaded,

		LoadComplete = ContentLoaded
	} status;

	HMODULE dll;

	//char const* name;

	//unsigned int versionID;
	//char const* versionString;

	//char const* const* dependencies;
	//unsigned int dependencyCount;

	bool isLoaded();

	Plugin(const std::filesystem::path& path);
	~Plugin();

private:
	friend class PluginManager;

	std::filesystem::path path;

	inline bool _dllGood() { return dll != INVALID_HANDLE_VALUE; }

	void loadDLL();
	void registerContents();

	void unloadDLL();
};
