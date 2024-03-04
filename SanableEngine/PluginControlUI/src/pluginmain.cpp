#include "application/PluginCore.hpp"

#include "game/Game.hpp"
#include "game/GameObject.hpp"
#include "PluginManagerView.hpp"
#include "ShaderProgram.hpp"
#include "Material.hpp"

Game* game;
GameObject* ui;

ShaderProgram* uiShader;
Material* uiMaterial;

PLUGIN_C_API(bool) plugin_report(Plugin const* context, PluginReportedData* report, Application const* engine)
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
        uiShader = new ShaderProgram("resources/shaders/ui");
        uiShader->load();
        uiMaterial = new Material(uiShader);

        ui = game->addGameObject();
        ui->CreateComponent<PluginManagerView>(uiMaterial);
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

        delete uiMaterial;
        uiMaterial = nullptr;
        delete uiShader;
        uiShader = nullptr;

        game->getApplication()->getMemoryManager()->destroyPool<PluginManagerView>();
    }
}
