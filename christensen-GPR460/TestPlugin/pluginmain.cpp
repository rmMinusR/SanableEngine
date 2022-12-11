#include <iostream>

#include "PluginCore.h"
#include "Plugin.h"

PLUGIN_API(bool) registerPlugin(Plugin* context, EngineCore* engine)
{
    std::cout << "registerPlugin() called" << std::endl;
    return true;
}

PLUGIN_API(bool) initialize()
{
    std::cout << "initialize() called" << std::endl;
    return true;
}

PLUGIN_API(void) cleanup()
{
    std::cout << "cleanup() called" << std::endl;
}

PLUGIN_API(int) testExport()
{
    std::cout << "testExport() called" << std::endl;
    return 42;
}
