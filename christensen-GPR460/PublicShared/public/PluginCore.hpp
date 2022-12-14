#pragma once

struct Plugin;
class EngineCore;

#if _WIN32
#define PLUGIN_API_CALLCONV __cdecl
#define PLUGIN_API_KEEPALIVE
#define PLUGIN_API_EXPORT __declspec(dllexport)
#endif

#if __EMSCRIPTEN__
#include <emscripten.h>
#define PLUGIN_API_CALLCONV __cdecl
#define PLUGIN_API_KEEPALIVE EMSCRIPTEN_KEEPALIVE
#define PLUGIN_API_EXPORT
#endif

#define PLUGIN_API(returnType) PLUGIN_API_KEEPALIVE returnType PLUGIN_API_EXPORT PLUGIN_API_CALLCONV
#define PLUGIN_C_API(returnType) extern "C" PLUGIN_API(returnType)
#define PLUGIN_API_CTOR PLUGIN_API_KEEPALIVE PLUGIN_API_EXPORT

//This file should be included by plugins so they can implement the following functions
//Also put PLUGIN_API_ATTRIBS after exported function bodies

PLUGIN_C_API(bool) plugin_preInit(Plugin* context, EngineCore* engine);
PLUGIN_C_API(bool) plugin_init();
PLUGIN_C_API(void) plugin_cleanup();

typedef bool (PLUGIN_API_CALLCONV *fp_plugin_preInit)(Plugin* context, EngineCore* engine);
typedef bool (PLUGIN_API_CALLCONV *fp_plugin_init   )();
typedef void (PLUGIN_API_CALLCONV *fp_plugin_cleanup)();
