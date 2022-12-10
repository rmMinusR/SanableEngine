#pragma once

#include "PluginInfo.h"
#include "PluginLoader.h"

//This file should be included by plugins so they can implement the following functions

bool __declspec(dllexport) __stdcall registerPlugin(PluginLoader* loader);
bool __declspec(dllexport) __stdcall initialize();
bool __declspec(dllexport) __stdcall loadContent();
void __declspec(dllexport) __stdcall cleanup();

typedef bool (__stdcall *fp_registerPlugin)(PluginLoader* loader);
typedef bool (__stdcall *fp_initialize    )();
typedef bool (__stdcall *fp_loadContent   )();
typedef void (__stdcall *fp_cleanup       )();
