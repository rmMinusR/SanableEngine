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
#include "Sprite.hpp"
#include "Font.hpp"
#include "Resources.hpp"
#include "gui/UISprite.hpp"

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
            WindowBuilder builder = game->getApplication()->buildWindow("Plugin Control", 800, 600);
            WindowGUIRenderPipeline* renderer = new WindowGUIRenderPipeline(game->getApplication());
            builder.setRenderPipeline(renderer);
            ctlGuiRoot = &renderer->hud;
            builder.setInputProcessor(new WindowGUIInputProcessor(ctlGuiRoot, 5));
            ctlWindow = builder.build();
        }

        //Resource loading must be done after creating Window or we get code 1282 (invalid operation)

        //Ready resources: images
        Resources::buttonNormalTexture = ctlWindow->getRenderer()->loadTexture("resources/ui/textures/button/normal.png");
        Resources::buttonNormalSprite = new UISprite3x3(Resources::buttonNormalTexture);
        Resources::buttonPressedTexture = ctlWindow->getRenderer()->loadTexture("resources/ui/textures/button/normal_pressed.png");
        Resources::buttonPressedSprite = new UISprite3x3(Resources::buttonPressedTexture);
        Resources::buttonDisabledTexture = ctlWindow->getRenderer()->loadTexture("resources/ui/textures/button/disabled.png");
        Resources::buttonDisabledSprite = new UISprite3x3(Resources::buttonDisabledTexture);

        Resources::rttiFieldTexture = ctlWindow->getRenderer()->loadTexture("resources/ui/textures/field.png");
        Resources::rttiFieldSprite = new UISprite3x3(Resources::rttiFieldTexture);
        Resources::rttiFieldSprite->setPixel({1,1}, {7,6});
        Resources::rttiFieldSprite->setPixel({2,2}, {8,8});

        Resources::rttiParentTexture = ctlWindow->getRenderer()->loadTexture("resources/ui/textures/parent.png");
        Resources::rttiParentSprite = new UISprite3x3(Resources::rttiParentTexture);

        //Ready resources: text
        ShaderProgram* textShader = new ShaderProgram("resources/ui/shaders/font");
        if (!textShader->load()) assert(false);
        Resources::textMat = new Material(textShader);
        Resources::headerFont = new Font("resources/ui/fonts/arial.ttf", 48);
        Resources::labelFont = new Font("resources/ui/fonts/arial.ttf", 24);

        //Init UI elements
        ui = ctlGuiRoot->addWidget<PluginManagerView>(game->getApplication()->getPluginManager(), nullptr);
        ui->transform.fillParent();
        
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
