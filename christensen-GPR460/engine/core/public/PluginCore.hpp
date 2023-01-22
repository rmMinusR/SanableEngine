#pragma once

#include "dllapi.h"

#include <vector>
#include "Hotswap.inl"

struct Plugin;
struct PluginReportedData;
class EngineCore;

#if _WIN32
#define PLUGIN_API_KEEPALIVE
#endif

#if __EMSCRIPTEN__
#include <emscripten.h>
#define PLUGIN_API_KEEPALIVE EMSCRIPTEN_KEEPALIVE
#endif

#define PLUGIN_C_API_SPEC __cdecl
#define PLUGIN_C_API(returnVal) extern "C" PLUGIN_API_KEEPALIVE returnVal __declspec(dllexport) PLUGIN_C_API_SPEC

//This file should be included by plugins so they can implement the following functions

PLUGIN_C_API(bool) plugin_preInit(Plugin* const context, PluginReportedData* report, EngineCore* engine);
PLUGIN_C_API(bool) plugin_init(bool firstRun);
PLUGIN_C_API(void) plugin_cleanup(bool shutdown);

typedef bool (__cdecl *fp_plugin_preInit)(Plugin* const context, PluginReportedData* report, EngineCore* engine);
typedef bool (__cdecl *fp_plugin_init   )(bool firstRun);
typedef void (__cdecl *fp_plugin_cleanup)(bool shutdown);

struct PluginReportedData
{
	std::string name;

	//unsigned int versionID;
	//std::string versionString;

	//std::vector<std::string> dependencies;

	std::vector<HotswapTypeData> hotswappables;
};
