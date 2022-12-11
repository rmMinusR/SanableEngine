#pragma once

struct Plugin;
class EngineCore;

//This file should be included by plugins so they can implement the following functions

extern "C" bool __declspec(dllexport) __stdcall registerPlugin(Plugin* context, EngineCore* engine);
extern "C" bool __declspec(dllexport) __stdcall initialize();
extern "C" bool __declspec(dllexport) __stdcall loadContent();
extern "C" void __declspec(dllexport) __stdcall cleanup();

typedef bool (__stdcall *fp_registerPlugin)(Plugin* loader);
typedef bool (__stdcall *fp_initialize    )();
typedef bool (__stdcall *fp_loadContent   )();
typedef void (__stdcall *fp_cleanup       )();
