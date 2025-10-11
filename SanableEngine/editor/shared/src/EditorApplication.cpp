#include "EditorApplication.hpp"

#include "game/Game.hpp"
#include "System.hpp"
#include "Window.hpp"
#include "MemoryRoot.hpp"
#include "Renderer.hpp"
#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "Font.hpp"
#include "gui/HUD.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/LabelWidget.hpp"
#include "gui/ButtonWidget.hpp"
#include "gui/WidgetTransform.hpp"
#include "gui/HorizontalGroupWidget.hpp"
#include "gui/VerticalGroupWidget.hpp"
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

    // Load text shader
    ShaderProgram* textShader = editorWindow->getRenderer()->loadShaderProgram(system->GetBaseDir() / "resources/ui/shaders/font");
    if (!textShader->load()) assert(false);
    Material* textMat = new Material(textShader);
    textMat->setGroup(Material::Group::Transparent);

    // Load text font
    Font* fontTabHeading = new Font(system->GetBaseDir() / "resources/ui/fonts/arial.ttf", 24);

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

    // Add widgets

    //  |-------------------------|
    //  |       |                 |
    //  | Hier. |   Scene/game    |
    //  |       |                 |
    //  |-------------------------|
    //  |         Files           |
    //  |-------------------------|

    VerticalGroupWidget* rootGroup = hud->addWidget<VerticalGroupWidget>();
    rootGroup->getTransform()->setPositioningStrategy<AnchoredPositioning>()->fillParent();

    // Top part: hierarchy, scene, and game views
    HorizontalGroupWidget* curLevelGroup = hud->addWidget<HorizontalGroupWidget>();
    curLevelGroup->getTransform()->setParent(rootGroup->getTransform());
    curLevelGroup->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(rootGroup);

    RadioButtonWidget::SpriteSet tabSprites = { sprPlaceholder1, sprPlaceholder2, sprPlaceholder2 };

    // Top-left: Hierarchy views (NYI)
    DraggableTabView* hierarchyTabArea = hud->addWidget<DraggableTabView>(Vector2f{ 120, 35 }, imageMat, tabSprites, TabView::TabsLocation::Top);
    hierarchyTabArea->getTransform()->setParent(curLevelGroup->getTransform());
    hierarchyTabArea->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(curLevelGroup);

    ImageWidget* objectHierarchyView = hud->addWidget<ImageWidget>(imageMat, sprPlaceholder1);
    {
        RadioButtonWidget* objectHierarchyButton = hierarchyTabArea->addItem(objectHierarchyView);
        LabelWidget* lbl = hud->addWidget<LabelWidget>(textMat, fontTabHeading, Color4<uint8_t>{ 0, 0, 0, 255 });
        lbl->getTransform()->setParent(objectHierarchyButton->getContentArea());
        lbl->getTransform()->setPositioningStrategy<AnchoredPositioning>()->fillParent(5);
        lbl->align = UIAnchor::centered();
        lbl->setText(L"Hierarchy");
    }

    ImageWidget* componentHierarchyView = hud->addWidget<ImageWidget>(imageMat, sprPlaceholder1);
    {
        RadioButtonWidget* componentHierarchyButton = hierarchyTabArea->addItem(componentHierarchyView);
        LabelWidget* lbl = hud->addWidget<LabelWidget>(textMat, fontTabHeading, Color4<uint8_t>{ 0, 0, 0, 255 });
        lbl->getTransform()->setParent(componentHierarchyButton->getContentArea());
        lbl->getTransform()->setPositioningStrategy<AnchoredPositioning>()->fillParent(5);
        lbl->align = UIAnchor::centered();
        lbl->setText(L"Details");
    }

    // Top left/right divider
    {
        ImageWidget* handleBackground = hud->addWidget<ImageWidget>(imageMat, sprPlaceholder1);
        GroupResizeHandle* worldGroupHandle = hud->addWidget<GroupResizeHandle>(handleBackground, sprPlaceholder1, sprPlaceholder2);
        worldGroupHandle->getTransform()->setParent(curLevelGroup->getTransform());
        worldGroupHandle->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(curLevelGroup)->config.setFixedSize(5);
    }

    // Top-right: Scene and game views (NYI)
    DraggableTabView* worldTabArea = hud->addWidget<DraggableTabView>(Vector2f{ 120, 35 }, imageMat, tabSprites, TabView::TabsLocation::Top);
    worldTabArea->getTransform()->setParent(curLevelGroup->getTransform());
    worldTabArea->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(curLevelGroup);

    // Scene view
    ImageWidget* sceneView = hud->addWidget<ImageWidget>(imageMat, sprPlaceholder1);
    {
        RadioButtonWidget* sceneViewButton = worldTabArea->addItem(sceneView);
        LabelWidget* lbl = hud->addWidget<LabelWidget>(textMat, fontTabHeading, Color4<uint8_t>{ 0, 0, 0, 255 });
        lbl->getTransform()->setParent(sceneViewButton->getContentArea());
        lbl->getTransform()->setPositioningStrategy<AnchoredPositioning>()->fillParent(5);
        lbl->align = UIAnchor::centered();
        lbl->setText(L"Scene");
    }

    // Game view - TODO do Unreal-style detach instead?
    ImageWidget* gameView = hud->addWidget<ImageWidget>(imageMat, sprPlaceholder1);
    {
        RadioButtonWidget* gameViewButton = worldTabArea->addItem(gameView);
        LabelWidget* lbl = hud->addWidget<LabelWidget>(textMat, fontTabHeading, Color4<uint8_t>{ 0, 0, 0, 255 });
        lbl->getTransform()->setParent(gameViewButton->getContentArea());
        lbl->getTransform()->setPositioningStrategy<AnchoredPositioning>()->fillParent(5);
        lbl->align = UIAnchor::centered();
        lbl->setText(L"Game");
    }

    // Top/bottom divider
    {
        ImageWidget* handleBackground = hud->addWidget<ImageWidget>(imageMat, sprPlaceholder1);
        GroupResizeHandle* rootGroupHandle = hud->addWidget<GroupResizeHandle>(handleBackground, sprPlaceholder1, sprPlaceholder2);
        rootGroupHandle->getTransform()->setParent(rootGroup->getTransform());
        rootGroupHandle->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(rootGroup)->config.setFixedSize(5);
    }

    // Bottom part: file view (NYI)
    ImageWidget* fileView = hud->addWidget<ImageWidget>(imageMat, sprPlaceholder1);
    fileView->getTransform()->setParent(rootGroup->getTransform());
    fileView->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(rootGroup);
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
    system->DoMainLoop(+[](Application* arg) { static_cast<EditorApplication*>(arg)->frameStep(); }, (Application*)this);
}

void EditorApplication::frameStep()
{
    if (!currentGame || currentGamePaused) system->pumpEvents();
    else currentGameStepFn(currentGame); // Calls pumpEvents on the System facade, which just passes through to root System

    // Other than play-in-editor, the editor is an entirely
    // GUI application, so this doubles as tick for all UI
    for (size_t i = 0; i < system->getNumWindows(); ++i) system->getWindow(i)->draw();

    if (pluginManager.executeCommandBuffer() != 0)
    {
        MemoryRoot::get()->ensureFresh();
    }
}

void EditorApplication::startPlayInEditor()
{
    assert(!gameSystem);
    gameSystem = new System_PlayInEditor(Application::system, this);
    gameSystem->Init();

    assert(!currentGame);
    currentGame = gameCode->SanableMain(system);
    currentGamePaused = false;
}

void EditorApplication::stopPlayInEditor(bool force)
{
    gameSystem->requestQuit();

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

void EditorApplication::tickCurrentGame()
{
    assert(currentGame);
    currentGame->frameStep();
}

void EditorApplication::setCurrentGamePaused(bool paused)
{
    currentGamePaused = paused;
}

bool EditorApplication::isCurrentGamePaused() const
{
    return currentGamePaused;
}
