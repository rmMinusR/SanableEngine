#include <iostream>

#include "PluginCore.hpp"
#include "Plugin.hpp"

#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"

PLUGIN_C_API(bool) plugin_preInit(Plugin* const context, PluginReportedData* report, EngineCore* engine)
{
    std::cout << "PrimitivesPlugin: plugin_preInit() called" << std::endl;

    report->name = "PrimitivesPlugin";

    report->hotswappables.push_back(HotswapTypeData::build<RectangleCollider>(0, 0));
    report->hotswappables.push_back(HotswapTypeData::build<RectangleRenderer>(0, 0, SDL_Color{}));

    return true;
}

PLUGIN_C_API(bool) plugin_init()
{
    std::cout << "PrimitivesPlugin: plugin_init() called" << std::endl;
    return true;
}

PLUGIN_C_API(void) plugin_cleanup()
{
    std::cout << "PrimitivesPlugin: plugin_cleanup() called" << std::endl;
}
