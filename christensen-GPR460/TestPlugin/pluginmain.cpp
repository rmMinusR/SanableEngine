#include "PluginCore.h"

#include "exports.h"

bool __declspec(dllexport) __stdcall registerPlugin(PluginLoader* loader)
{
    return true;
}

bool __declspec(dllexport) __stdcall initialize()
{
    return true;
}

bool __declspec(dllexport) __stdcall loadContent()
{
    return true;
}

void __declspec(dllexport) __stdcall cleanup()
{
    
}

int __declspec(dllexport) __stdcall testExport()
{
    return 42;
}
int(__stdcall* TestPlugin::testExport)() = testExport;