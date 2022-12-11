#pragma once

struct Plugin;
class EngineCore;

#if _WIN32
#define PLUGIN_API_CALLCONV __cdecl
#define PLUGIN_API(returnType) extern "C" returnType __declspec(dllexport) PLUGIN_API_CALLCONV
#endif

#if __EMSCRIPTEN__
#include <emscripten.h>
#define PLUGIN_API_CALLCONV __cdecl
#define PLUGIN_API(returnType) extern "C" EMSCRIPTEN_KEEPALIVE returnType PLUGIN_API_CALLCONV
#endif

//This file should be included by plugins so they can implement the following functions
//Also put PLUGIN_API_ATTRIBS after exported function bodies

PLUGIN_API(bool) plugin_preInit(Plugin* context, EngineCore* engine);
PLUGIN_API(bool) plugin_init();
PLUGIN_API(void) plugin_cleanup();

typedef bool (PLUGIN_API_CALLCONV *fp_plugin_preInit)(Plugin* context, EngineCore* engine);
typedef bool (PLUGIN_API_CALLCONV *fp_plugin_init   )();
typedef void (PLUGIN_API_CALLCONV *fp_plugin_cleanup)();
