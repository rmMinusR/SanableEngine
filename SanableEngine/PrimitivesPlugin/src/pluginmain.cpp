#include <iostream>

#include "PluginCore.hpp"
#include "Plugin.hpp"

#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"

EngineCore* engine;

PLUGIN_C_API(bool) plugin_preInit(Plugin* const context, PluginReportedData* report, EngineCore* engine)
{
    printf("PrimitivesPlugin: plugin_preInit() called\n");

    report->name = "PrimitivesPlugin";

    report->types.registerType<RectangleCollider>(0, 0);
    report->types.registerType<RectangleRenderer>(0, 0, SDL_Color{});
    
    ::engine = engine;

    return true;
}

PLUGIN_C_API(bool) __cdecl plugin_init(bool firstRun)
{
    printf("PrimitivesPlugin: plugin_init() called\n");
    return true;
}

PLUGIN_C_API(void) __cdecl plugin_cleanup(bool shutdown)
{
    printf("PrimitivesPlugin: plugin_cleanup() called\n");

    if (shutdown)
    {
        engine->getMemoryManager()->destroyPool<RectangleCollider>();
        engine->getMemoryManager()->destroyPool<RectangleRenderer>();
    }
}
