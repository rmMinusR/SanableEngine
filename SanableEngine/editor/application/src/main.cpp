#include <thread>
#include <chrono>
using namespace std::chrono_literals;

#include "MemoryRoot.hpp"
#include "System.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "gui/GUIWindowDispatcher.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/ButtonWidget.hpp"
#include "gui/WidgetTransform.hpp"
#include "gui/HorizontalGroupWidget.hpp"
#include "gui/UISprite.hpp"
#include "EditorApplication.hpp"
#include "DraggableTabView.hpp"
#include "GroupResizeHandle.hpp"
#include "SplashWindow.hpp"

void SanableMain(gpr460::System* system)
{
    // Init
    EditorApplication editor(*system);
    MemoryRoot::get()->registerExternal(&editor, ExternalObjectOptions::DefaultExternal);
    editor.init();

    // Setup splash window
    SplashWindow* loaderWindow = new SplashWindow(&editor, L"Sanable Editor - Loading...", { 300, 200 });
    editor.setMainWindow(loaderWindow->getWindow());
    system->pumpEvents();
    loaderWindow->redraw();

    // Dummy loading logic (placeholder for later)
    std::this_thread::sleep_for(2s);

    // Setup editor window
    {
        WindowSettings mainWindowSettings("Sanable Editor", 800, 600);
        GUIWindowDispatcher* editorWindowLogic = new GUIWindowDispatcher(&editor, 5);
        mainWindowSettings.userLogic = editorWindowLogic;

        Window* editorWindow = editor.buildWindow(mainWindowSettings);
        editorWindow->getRenderer()->activate();

        // Load image shader
        ShaderProgram* imageShader = editorWindow->getRenderer()->loadShaderProgram( system->GetBaseDir() / "resources/ui/shaders/image");
        if (!imageShader->load()) assert(false);
        Material* imageMat = new Material(imageShader);

        // Load placeholder sprites
        GTexture* texPlaceholder1 = editorWindow->getRenderer()->loadTexture(system->GetBaseDir() / "resources/ui/textures/placeholder_1.png");
        GTexture* texPlaceholder2 = editorWindow->getRenderer()->loadTexture(system->GetBaseDir() / "resources/ui/textures/placeholder_2.png");
        UISprite3x3* sprPlaceholder1 = new UISprite3x3(texPlaceholder1);
        sprPlaceholder1->setPixel({1,1}, {7,5});
        sprPlaceholder1->setPixel({1,1}, {58,58});
        UISprite3x3* sprPlaceholder2 = new UISprite3x3(texPlaceholder2);
        sprPlaceholder2->setPixel({1,1}, {7,5});
        sprPlaceholder2->setPixel({1,1}, {58,58});

        // Load resize divider sprites
        GTexture* texDividerNormal  = editorWindow->getRenderer()->loadTexture(system->GetBaseDir() / "resources/ui/textures/divider_h/normal.png");
        GTexture* texDividerDragged = editorWindow->getRenderer()->loadTexture(system->GetBaseDir() / "resources/ui/textures/divider_h/dragged.png");
        UISprite3x3* sprDividerNormal = new UISprite3x3(texDividerNormal);
        sprDividerNormal->setPixel({1,1}, {3,5});
        sprDividerNormal->setPixel({2,2}, {4,8});
        UISprite3x3* sprDividerDragged = new UISprite3x3(texDividerDragged);
        sprDividerDragged->setPixel({ 1,1 }, { 3,5 });
        sprDividerDragged->setPixel({ 2,2 }, { 4,8 });

        HUD* hud = &editorWindowLogic->hud;
        HorizontalGroupWidget* hgrp = hud->addWidget<HorizontalGroupWidget>();
        hgrp->getTransform()->setPositioningStrategy<AnchoredPositioning>()->fillParent();

        for (size_t i = 0; i < 2; ++i)
        {
            DraggableTabView* tabView = hud->addWidget<DraggableTabView>(Vector2f{ 100, 50 }, imageMat, RadioButtonWidget::SpriteSet { sprPlaceholder1, sprPlaceholder2, sprPlaceholder2 }, TabView::TabsLocation::Top);
            tabView->getTransform()->setParent(hgrp->getTransform());
            tabView->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(hgrp);
        
            ImageWidget* placeholderLeft = hud->addWidget<ImageWidget>(imageMat, sprPlaceholder1); // Left
            RadioButtonWidget* btnLeft = tabView->addItem(placeholderLeft);

            ImageWidget* placeholderRight = hud->addWidget<ImageWidget>(imageMat, sprPlaceholder2); // Right
            RadioButtonWidget* btnRight = tabView->addItem(placeholderRight);
        }

        editorWindow->draw();
        editor.setMainWindow(editorWindow);
    }

    // Teardown loader window and UI resources
    //delete loaderWindow;

    editor.doMainLoop();

    // Shutdown
    editor.cleanup();
}
