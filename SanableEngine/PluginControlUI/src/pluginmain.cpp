#include "application/PluginCore.hpp"

#include "game/Game.hpp"
#include "game/GameObject.hpp"
#include "gui/WindowGUIRenderPipeline.hpp"
#include "application/Window.hpp"
#include "PluginManagerView.hpp"
#include "ShaderProgram.hpp"
#include "Material.hpp"

Game* game;
PluginManagerView* ui;
Window* ctlWindow;
HUD* ctlGuiRoot;

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
            WindowGUIRenderPipeline* _renderer = new WindowGUIRenderPipeline();
            WindowBuilder builder = game->getApplication()->buildWindow("Plugin Control", 800, 600, std::move(_renderer));
            ctlWindow = builder.build();
        }

        Renderer::errorCheck();

        //Load UI shader/material. Must be done after creating Window or we get code 1282 (invalid operation)
        //uiShader = new ShaderProgram("resources/shaders/ui");
        //if (!uiShader->load()) Renderer::errorCheck();
        //uiMaterial = new Material(uiShader);

        ctlGuiRoot = &static_cast<WindowGUIRenderPipeline*>(ctlWindow->getRenderPipeline())->hud;
        //ui = ctlGuiRoot->addWidget<PluginManagerView>(game->getApplication()->getPluginManager(), uiMaterial);

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
        ctlGuiRoot->removeWidget(ui);
        ui = nullptr;

        delete uiMaterial;
        uiMaterial = nullptr;
        delete uiShader;
        uiShader = nullptr;

        delete ctlWindow;
        ctlWindow = nullptr;
    }
}
