#include <iostream>

#include "PluginCore.h"
#include "Plugin.h"

PLUGIN_API(bool) plugin_preInit(Plugin* context, EngineCore* engine)
{
    std::cout << "DLL: plugin_preInit() called" << std::endl;
    return true;
}

PLUGIN_API(bool) plugin_init()
{
    std::cout << "DLL: plugin_init() called" << std::endl;
    return true;
}

PLUGIN_API(void) plugin_cleanup()
{
    std::cout << "DLL: plugin_cleanup() called" << std::endl;
}

PLUGIN_API(int) testExport()
{
    std::cout << "DLL: testExport() called" << std::endl;
    return 42;
}
