#include <iostream>

#include "PluginCore.hpp"
#include "Plugin.hpp"

PLUGIN_API(bool) plugin_preInit(Plugin* context, EngineCore* engine)
{
    std::cout << "PrimitivesPlugin: plugin_preInit() called" << std::endl;
    return true;
}

PLUGIN_API(bool) plugin_init()
{
    std::cout << "PrimitivesPlugin: plugin_init() called" << std::endl;
    return true;
}

PLUGIN_API(void) plugin_cleanup()
{
    std::cout << "PrimitivesPlugin: plugin_cleanup() called" << std::endl;
}
