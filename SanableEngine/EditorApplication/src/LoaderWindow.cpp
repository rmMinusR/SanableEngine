#include "LoaderWindow.hpp"

#include "Window.hpp"
#include "System.hpp"
#include "Font.hpp"
#include "Material.hpp"
#include "Renderer.hpp"
#include "ShaderProgram.hpp"
#include "application/Application.hpp"
#include "gui/GUIWindowDispatcher.hpp"
#include "gui/LabelWidget.hpp"
#include "gui/ImageWidget.hpp"

LoaderWindow::LoaderWindow(Application* application, std::wstring title, Vector2<int> size)
{
    this->application = application;

    WindowSettings loaderWindowSettings("Sanable Editor - Loading...", size.x, size.y);
    GUIWindowDispatcher* dispatcher = new GUIWindowDispatcher(application, 5);
    HUD* uiRoot = &dispatcher->hud;

    loaderWindowSettings.userLogic = dispatcher;
    window = application->buildWindow(loaderWindowSettings);

    // Setup UI resources
    textShader = window->getRenderer()->loadShaderProgram(application->getSystem()->GetBaseDir() / "resources/ui/shaders/font");
    if (!textShader->load()) assert(false);
    textMat = new Material(textShader);
    textFont = new Font(application->getSystem()->GetBaseDir() / "resources/ui/fonts/arial.ttf", 48);

    // Setup UI
    uiRoot->addWidget<LabelWidget>(textMat, textFont);
}

LoaderWindow::~LoaderWindow()
{
    delete textFont;
    delete textMat;
    delete textShader;
    application->getSystem()->destroyWindow(window);
}

void LoaderWindow::redraw()
{
    window->draw();
}

Window* LoaderWindow::getWindow()
{
    return window;
}
