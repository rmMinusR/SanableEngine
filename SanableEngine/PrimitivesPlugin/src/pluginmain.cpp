#include <iostream>

#include "application/PluginCore.hpp"
#include "application/Plugin.hpp"

#include "RectangleCollider.hpp"
#include "RectangleRenderer.hpp"

Application* application;

PLUGIN_C_API(bool) plugin_report(Plugin const* context, PluginReportedData* report, Application const* application)
{
    printf("PrimitivesPlugin: plugin_report() called\n");

    report->name = L"PrimitivesPlugin";
    
    ::application = (Application*)application; //FIXME bad practice

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
    }
}
