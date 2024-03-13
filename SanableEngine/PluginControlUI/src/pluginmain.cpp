#include "application/PluginCore.hpp"

#include "game/Game.hpp"
#include "game/GameObject.hpp"
#include "gui/WindowGUIRenderPipeline.hpp"
#include "gui/WindowGUIInputProcessor.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/LabelWidget.hpp"
#include "gui/ButtonWidget.hpp"
#include "application/Window.hpp"
#include "PluginManagerView.hpp"
#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include "Font.hpp"
#include "Resources.hpp"

Game* game;
PluginManagerView* ui;
Window* ctlWindow;
HUD* ctlGuiRoot;

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
            WindowGUIRenderPipeline* renderer = new WindowGUIRenderPipeline();
            WindowBuilder builder = game->getApplication()->buildWindow("Plugin Control", 800, 600, renderer);
            ctlGuiRoot = &renderer->hud;
            builder.setInputProcessor(new WindowGUIInputProcessor(ctlGuiRoot));
            ctlWindow = builder.build();
        }

        //Resource loading must be done after creating Window or we get code 1282 (invalid operation)

        //Ready resources: images
        ShaderProgram* uiShader = new ShaderProgram("resources/shaders/ui");
        uiShader->load();
        Resources::imageMat = new Material(uiShader);

        //Ready resources: text
        ShaderProgram* textShader = new ShaderProgram("resources/ui/shaders/font");
        if (!textShader->load()) assert(false);
        Resources::textMat = new Material(textShader);
        Resources::headerFont = new Font("resources/ui/fonts/arial.ttf", 48);
        Resources::labelFont = new Font("resources/ui/fonts/arial.ttf", 24);

        //Init UI elements
        ui = ctlGuiRoot->addWidget<PluginManagerView>(game->getApplication()->getPluginManager(), nullptr);
        
        Texture* texture = ctlWindow->getRenderer()->loadTexture("resources/ui/textures/blue_boxCheckmark.png");
        ImageWidget* buttonBg = ctlGuiRoot->addWidget<ImageWidget>(nullptr, texture);

        LabelWidget* buttonLabel = ctlGuiRoot->addWidget<LabelWidget>(Resources::textMat, Resources::labelFont);
        buttonLabel->setText(L"Text test!");

        ButtonWidget* btn = ctlGuiRoot->addWidget<ButtonWidget>(buttonBg, buttonLabel);
        btn->transform.setMinCornerOffset({ -100, -50 });
        btn->transform.setMaxCornerOffset({  100,  50 });
        btn->setCallback([]() {
            printf("Button clicked!\n");
        });

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

        delete ctlWindow;
        ctlWindow = nullptr;
    }
}
