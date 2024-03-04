#include "application/PluginCore.hpp"

#include "game/Game.hpp"
#include "game/GameObject.hpp"
#include "PluginManagerView.hpp"

Game* game;
GameObject* ui;

PLUGIN_C_API(bool) plugin_preInit(Plugin const* context, PluginReportedData* report, Application const* engine)
{
    printf("PluginControlUI: plugin_preInit() called\n");

    report->name = L"PluginControlUI";
    
    ::game = engine->getGame();
    
    return true;
}

PLUGIN_C_API(bool) __cdecl plugin_init(bool firstRun)
{
    printf("PluginControlUI: plugin_init() called\n");

    if (firstRun)
    {
        ui = game->addGameObject();
        ui->CreateComponent<PluginManagerView>();
    }

    return true;
}

PLUGIN_C_API(void) __cdecl plugin_cleanup(bool shutdown)
{
    printf("PluginControlUI: plugin_cleanup() called\n");

    if (shutdown)
    {
        game->destroy(ui);
        ui = nullptr;
    }
}
