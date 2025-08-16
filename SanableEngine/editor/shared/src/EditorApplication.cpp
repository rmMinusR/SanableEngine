#include "EditorApplication.hpp"

#include "game/Game.hpp"
#include "System.hpp"
#include "Window.hpp"
#include "MemoryRoot.hpp"
#include "Renderer.hpp"
#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "gui/HUD.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/ButtonWidget.hpp"
#include "gui/WidgetTransform.hpp"
#include "gui/HorizontalGroupWidget.hpp"
#include "gui/UISprite.hpp"
#include "DraggableTabView.hpp"
#include "GroupResizeHandle.hpp"
#include "System_PlayInEditor.hpp"

EditorApplication::EditorApplication(std::filesystem::path projectDir) :
	Application(),
    projectDir(projectDir)
{
}

EditorApplication::~EditorApplication()
{
}

void EditorApplication::init(gpr460::System& system)
{
    Application::init(system);
}

void EditorApplication::setupDefaultLayout(Window* editorWindow, HUD* hud)
{
    editorWindow->getRenderer()->activate();

    // Load image shader
    ShaderProgram* imageShader = editorWindow->getRenderer()->loadShaderProgram(system->GetBaseDir() / "resources/ui/shaders/image");
    if (!imageShader->load()) assert(false);
    Material* imageMat = new Material(imageShader);

    // Load placeholder sprites
    GTexture* texPlaceholder1 = editorWindow->getRenderer()->loadTexture(system->GetBaseDir() / "resources/ui/textures/placeholder_1.png");
    GTexture* texPlaceholder2 = editorWindow->getRenderer()->loadTexture(system->GetBaseDir() / "resources/ui/textures/placeholder_2.png");
    UISprite3x3* sprPlaceholder1 = new UISprite3x3(texPlaceholder1);
    sprPlaceholder1->setPixel({ 1,1 }, { 7,5 });
    sprPlaceholder1->setPixel({ 1,1 }, { 58,58 });
    UISprite3x3* sprPlaceholder2 = new UISprite3x3(texPlaceholder2);
    sprPlaceholder2->setPixel({ 1,1 }, { 7,5 });
    sprPlaceholder2->setPixel({ 1,1 }, { 58,58 });

    // Load resize divider sprites
    GTexture* texDividerNormal = editorWindow->getRenderer()->loadTexture(system->GetBaseDir() / "resources/ui/textures/divider_h/normal.png");
    GTexture* texDividerDragged = editorWindow->getRenderer()->loadTexture(system->GetBaseDir() / "resources/ui/textures/divider_h/dragged.png");
    UISprite3x3* sprDividerNormal = new UISprite3x3(texDividerNormal);
    sprDividerNormal->setPixel({ 1,1 }, { 3,5 });
    sprDividerNormal->setPixel({ 2,2 }, { 4,8 });
    UISprite3x3* sprDividerDragged = new UISprite3x3(texDividerDragged);
    sprDividerDragged->setPixel({ 1,1 }, { 3,5 });
    sprDividerDragged->setPixel({ 2,2 }, { 4,8 });

    HorizontalGroupWidget* hgrp = hud->addWidget<HorizontalGroupWidget>();
    hgrp->getTransform()->setPositioningStrategy<AnchoredPositioning>()->fillParent();

    for (size_t i = 0; i < 2; ++i)
    {
        DraggableTabView* tabView = hud->addWidget<DraggableTabView>(Vector2f{ 100, 50 }, imageMat, RadioButtonWidget::SpriteSet{ sprPlaceholder1, sprPlaceholder2, sprPlaceholder2 }, TabView::TabsLocation::Top);
        tabView->getTransform()->setParent(hgrp->getTransform());
        tabView->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(hgrp);

        ImageWidget* placeholderLeft = hud->addWidget<ImageWidget>(imageMat, sprPlaceholder1); // Left
        RadioButtonWidget* btnLeft = tabView->addItem(placeholderLeft);

        ImageWidget* placeholderRight = hud->addWidget<ImageWidget>(imageMat, sprPlaceholder2); // Right
        RadioButtonWidget* btnRight = tabView->addItem(placeholderRight);
    }
}

void EditorApplication::cleanup()
{
    Application::cleanup();
}

void EditorApplication::doMainLoop()
{
    // Ensure up to date
    pluginManager.executeCommandBuffer();
    //refreshCallBatchers(true); // TODO: load bearing 

    // Run
    system->DoMainLoop(+[](void* arg) { static_cast<EditorApplication*>(arg)->frameStep(); }, this);
}

void EditorApplication::frameStep()
{
    if (!currentGame || currentGamePaused) system->pumpEvents();
    else currentGame->frameStep(); // Calls pumpEvents on the System facade, which just passes through to root System

    // Other than play-in-editor, the editor is an entirely
    // GUI application, so this doubles as tick for all UI
    for (size_t i = 0; i < system->getNumWindows(); ++i) system->getWindow(i)->draw();

    if (pluginManager.executeCommandBuffer() != 0)
    {
        MemoryRoot::get()->ensureFresh();
    }
}

void EditorApplication::startPlayInEditor(Game* game)
{
    assert(!gameSystem);
    gameSystem = new System_PlayInEditor(Application::system, this);
    gameSystem->Init();

    assert(!currentGame);
    currentGame = game;
    currentGame->init(*gameSystem);
    currentGamePaused = false;
}

void EditorApplication::stopPlayInEditor()
{
    currentGame->cleanup();

    assert(gameSystem);
    gameSystem->Shutdown();
    delete gameSystem;
}

std::filesystem::path EditorApplication::getProjectDir()
{
    return projectDir;
}

Game* EditorApplication::getCurrentGame()
{
    return currentGame;
}
