#include <iostream>

#include "application/PluginCore.hpp"
#include "application/Plugin.hpp"

#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"

Application* application;

PLUGIN_C_API(bool) plugin_preInit(Plugin* const context, PluginReportedData* report, Application* application)
{
    printf("PrimitivesPlugin: plugin_preInit() called\n");

    report->name = "PrimitivesPlugin";
    
    ::application = application;

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
        application->getMemoryManager()->destroyPool<RectangleCollider>();
        application->getMemoryManager()->destroyPool<RectangleRenderer>();
    }
}
