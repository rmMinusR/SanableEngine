#pragma once

struct PluginInfo
{
	char const* name;

	//unsigned int versionID;
	//char const* versionString;

	//char const* const* dependencies;
	//unsigned int dependencyCount;

	enum class Status
	{
		NotLoaded = 0,

		Registered,
		Initialized,
		ContentLoaded,

		LoadComplete = ContentLoaded
	} status;

	static PluginInfo* const get(char const* name);
};
