#include <iostream>

#include "PluginCore.hpp"
#include "Plugin.hpp"

#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"

PLUGIN_C_API(bool) plugin_preInit(Plugin* const context, PluginReportedData* report, EngineCore* engine)
{
    printf("PrimitivesPlugin: plugin_preInit() called\n");

    report->name = "PrimitivesPlugin";

    report->hotswappables.push_back(HotswapTypeData::build<RectangleCollider>(0, 0));
    report->hotswappables.push_back(HotswapTypeData::build<RectangleRenderer>(0, 0, SDL_Color{}));

    return true;
}

PLUGIN_C_API(bool) plugin_init()
{
    printf("PrimitivesPlugin: plugin_init() called\n");
    return true;
}

PLUGIN_C_API(void) plugin_cleanup()
{
    printf("PrimitivesPlugin: plugin_cleanup() called\n");
}
