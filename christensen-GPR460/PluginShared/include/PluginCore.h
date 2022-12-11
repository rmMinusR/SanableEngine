#pragma once

struct Plugin;
class EngineCore;

#define PLUGIN_API_CALL __cdecl
#define PLUGIN_API(returnType) extern "C" returnType __declspec(dllexport) PLUGIN_API_CALL

//This file should be included by plugins so they can implement the following functions

PLUGIN_API(bool) plugin_preInit(Plugin* context, EngineCore* engine);
PLUGIN_API(bool) plugin_init();
PLUGIN_API(void) plugin_cleanup();

typedef bool (PLUGIN_API_CALL *fp_plugin_preInit)(Plugin* context, EngineCore* engine);
typedef bool (PLUGIN_API_CALL *fp_plugin_init   )();
typedef void (PLUGIN_API_CALL *fp_plugin_cleanup)();
