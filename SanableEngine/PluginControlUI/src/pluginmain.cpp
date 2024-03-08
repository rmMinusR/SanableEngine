#include "application/PluginCore.hpp"

#include "game/Game.hpp"
#include "game/GameObject.hpp"
#include "game/gui/WindowGUIRenderPipeline.hpp"
#include "application/Window.hpp"
#include "PluginManagerView.hpp"
#include "ShaderProgram.hpp"
#include "Material.hpp"

Game* game;
PluginManagerView* ui;
Window* ctlWindow;

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
        {
            std::unique_ptr<WindowGUIRenderPipeline> renderer = std::make_unique<WindowGUIRenderPipeline>();
            WindowBuilder builder = game->getApplication()->buildWindow("Plugin Control", 800, 600, std::move(renderer));
            ctlWindow = builder.build();
        }

        //Load UI shader/material. Must be done after creating Window or we get code 1282 (invalid operation)
        uiShader = new ShaderProgram("resources/shaders/ui");
        uiShader->load();
        uiMaterial = new Material(uiShader);

        {
            WindowGUIRenderPipeline* renderer = (WindowGUIRenderPipeline*) ctlWindow->getRenderPipeline();
            ui = renderer->hud.addWidget<PluginManagerView>(game->getApplication()->getPluginManager(), uiMaterial);
        }

        //Restore main window context so rest of stuff can init properly
        //TODO do this (automatically?) at start of every plugin
        Window::setActiveDrawTarget(game->getApplication()->getMainWindow());
    }

    return true;
}

PLUGIN_C_API(void) __cdecl plugin_cleanup(bool shutdown)
{
    printf("PluginControlUI: plugin_cleanup() called\n");

    if (shutdown)
    {
        delete ctlWindow;
        ctlWindow = nullptr;

        delete uiMaterial;
        uiMaterial = nullptr;
        delete uiShader;
        uiShader = nullptr;
    }
}
