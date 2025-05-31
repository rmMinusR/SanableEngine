#include "application/PluginCore.hpp"

#include "game/Game.hpp"
#include "game/GameObject.hpp"
#include "gui/GUIWindowDispatcher.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/LabelWidget.hpp"
#include "gui/ButtonWidget.hpp"
#include "System.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
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
Plugin const* plugin;

PLUGIN_C_API(bool) plugin_report(Plugin const* context, PluginReportedData* report, Application const* engine)
{
    printf("PluginControlUI: plugin_preInit() called\n");

    report->name = L"PluginControlUI";
    
    ::game = engine->getGame();
    ::plugin = context;
    
    return true;
}

PLUGIN_C_API(bool) __cdecl plugin_init(bool firstRun)
{
    printf("PluginControlUI: plugin_init() called\n");

    if (firstRun)
    {
        {
            WindowSettings windowSettings("Plugin Control", 800, 600);
            GUIWindowDispatcher* windowLogic = new GUIWindowDispatcher(game->getApplication(), 5);
            windowSettings.userLogic = windowLogic;
            ctlWindow = game->getApplication()->buildWindow(windowSettings);
            ctlGuiRoot = &windowLogic->hud;
        }

        //Resource loading must be done after creating Window or we get code 1282 (invalid operation)

        //Ready resources: images
        Resources::imageShader = ctlWindow->getRenderer()->loadShaderProgram("resources/ui/shaders/image");
        if (!Resources::imageShader->load()) assert(false);
        Resources::imageMat = new Material(Resources::imageShader);
        Resources::imageMat->setGroup(Material::Group::Transparent);
        Resources::buttonNormalTexture = ctlWindow->getRenderer()->loadTexture(plugin->getPluginDir() / "resources/ui/textures/button/normal.png");
        Resources::buttonNormalSprite = new UISprite3x3(Resources::buttonNormalTexture);
        Resources::buttonPressedTexture = ctlWindow->getRenderer()->loadTexture(plugin->getPluginDir() / "resources/ui/textures/button/normal_pressed.png");
        Resources::buttonPressedSprite = new UISprite3x3(Resources::buttonPressedTexture);
        Resources::buttonDisabledTexture = ctlWindow->getRenderer()->loadTexture(plugin->getPluginDir() / "resources/ui/textures/button/disabled.png");
        Resources::buttonDisabledSprite = new UISprite3x3(Resources::buttonDisabledTexture);

        Resources::rttiFieldTexture = ctlWindow->getRenderer()->loadTexture(plugin->getPluginDir() / "resources/ui/textures/field.png");
        Resources::rttiFieldSprite = new UISprite3x3(Resources::rttiFieldTexture);
        Resources::rttiFieldSprite->setPixel({1,1}, {7,6});
        Resources::rttiFieldSprite->setPixel({2,2}, {8,8});

        Resources::rttiParentTexture = ctlWindow->getRenderer()->loadTexture(plugin->getPluginDir() / "resources/ui/textures/parent.png");
        Resources::rttiParentSprite = new UISprite3x3(Resources::rttiParentTexture);

        //Ready resources: text
        Resources::textShader = ctlWindow->getRenderer()->loadShaderProgram("resources/ui/shaders/font");
        if (!Resources::textShader->load()) assert(false);
        Resources::textMat = new Material(Resources::textShader);
        Resources::textMat->setGroup(Material::Group::Transparent);
        Resources::headerFont = new Font(plugin->getPluginDir() / "resources/ui/fonts/arial.ttf", 48);
        Resources::labelFont = new Font(plugin->getPluginDir() / "resources/ui/fonts/arial.ttf", 24);

        //Init UI elements
        ui = ctlGuiRoot->addWidget<PluginManagerView>(game->getApplication()->getPluginManager(), nullptr);
        ui->getTransform()->setPositioningStrategy<AnchoredPositioning>()->fillParent();
        
        //Restore main window context so rest of stuff can init properly
        //TODO do this (automatically?) at start of every plugin
        game->getApplication()->getMainWindow()->setActiveDrawTarget();
    }

    return true;
}

PLUGIN_C_API(void) __cdecl plugin_cleanup(bool shutdown)
{
    printf("PluginControlUI: plugin_cleanup() called\n");

    if (shutdown)
    {
        ctlGuiRoot->destroyWidget(ui);
        ui = nullptr;

        delete Resources::headerFont;
        delete Resources::labelFont;
        delete Resources::textMat;
        delete Resources::textShader;
        delete Resources::imageMat;
        delete Resources::imageShader;
        delete Resources::buttonNormalSprite;
        delete Resources::buttonNormalTexture;
        delete Resources::buttonPressedSprite;
        delete Resources::buttonPressedTexture;
        delete Resources::buttonDisabledSprite;
        delete Resources::buttonDisabledTexture;
        delete Resources::rttiFieldSprite;
        delete Resources::rttiFieldTexture;
        delete Resources::rttiParentSprite;
        delete Resources::rttiParentTexture;

        game->getApplication()->getSystem()->destroyWindow(ctlWindow);
        ctlWindow = nullptr;
    }
}
