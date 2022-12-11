#include "PluginCore.h"

extern "C" bool __declspec(dllexport) __stdcall registerPlugin(Plugin* context, EngineCore* engine)
{
    return true;
}

extern "C" bool __declspec(dllexport) __stdcall initialize()
{
    return true;
}

extern "C" bool __declspec(dllexport) __stdcall loadContent()
{
    return true;
}

extern "C" void __declspec(dllexport) __stdcall cleanup()
{
    
}

extern "C" int __declspec(dllexport) __stdcall testExport()
{
    return 42;
}
