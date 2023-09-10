#include "PluginCore.hpp"

#include "EngineCore.hpp"
#include "GameObject.hpp"
#include "PluginManagerView.hpp"

EngineCore* engine;
GameObject* ui;

PLUGIN_C_API(bool) plugin_preInit(Plugin* const context, PluginReportedData* report, EngineCore* engine)
{
    printf("PluginControlUI: plugin_preInit() called\n");

    report->name = "PluginControlUI";
    
    ::engine = engine;
    
    return true;
}

PLUGIN_C_API(bool) __cdecl plugin_init(bool firstRun)
{
    printf("PluginControlUI: plugin_init() called\n");

    if (firstRun)
    {
        ui = engine->addGameObject();
        ui->CreateComponent<PluginManagerView>();
    }

    return true;
}

PLUGIN_C_API(void) __cdecl plugin_cleanup(bool shutdown)
{
    printf("PluginControlUI: plugin_cleanup() called\n");

    if (shutdown)
    {
        engine->destroy(ui);
        ui = nullptr;
    }
}
