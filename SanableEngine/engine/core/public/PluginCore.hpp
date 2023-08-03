#pragma once

#include "dllapi.h"

#include <vector>
#include "ModuleTypeRegistry.inl"

struct Plugin;
struct PluginReportedData;
class EngineCore;

#define PLUGIN_C_API_SPEC __cdecl
#define PLUGIN_C_API(returnVal) extern "C" API_KEEPALIVE returnVal API_EXPORT PLUGIN_C_API_SPEC

//This file should be included by plugins so they can implement the following functions

PLUGIN_C_API(bool) plugin_preInit(Plugin* const context, PluginReportedData* report, EngineCore* engine);
PLUGIN_C_API(bool) plugin_init(bool firstRun);
PLUGIN_C_API(void) plugin_cleanup(bool shutdown);
PLUGIN_C_API(void) plugin_reportTypes(ModuleTypeRegistry* report); //Most plugins should autogenerate this. You could also write one by hand, or omit it entirely, but it's strongly recommended.

typedef bool (PLUGIN_C_API_SPEC *fp_plugin_preInit)(Plugin* const context, PluginReportedData* report, EngineCore* engine);
typedef bool (PLUGIN_C_API_SPEC *fp_plugin_init   )(bool firstRun);
typedef void (PLUGIN_C_API_SPEC *fp_plugin_cleanup)(bool shutdown);
typedef void (PLUGIN_C_API_SPEC *fp_plugin_reportTypes)(ModuleTypeRegistry* report); //Optional, but strongly recommended.

struct PluginReportedData
{
	std::string name;

	//unsigned int versionID;
	//std::string versionString;

	//std::vector<std::string> dependencies;
};
