#pragma once

struct Plugin;
class EngineCore;

#define PLUGIN_API_DECL __cdecl
#define PLUGIN_API(returnType) extern "C" returnType __declspec(dllexport) PLUGIN_API_DECL

//This file should be included by plugins so they can implement the following functions

PLUGIN_API(bool) registerPlugin(Plugin* context, EngineCore* engine);
PLUGIN_API(bool) initialize();
PLUGIN_API(void) cleanup();

typedef bool (PLUGIN_API_DECL *fp_registerPlugin)(Plugin* context);
typedef bool (PLUGIN_API_DECL *fp_initialize    )();
typedef void (PLUGIN_API_DECL *fp_cleanup       )();
