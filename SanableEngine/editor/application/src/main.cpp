#include <thread>
#include <chrono>
using namespace std::chrono_literals;

#include "MemoryRoot.hpp"
#include "System.hpp"
#include "Window.hpp"
#include "EditorApplication.hpp"
#include "SplashWindow.hpp"
#include "gui/GUIWindowDispatcher.hpp"

Application* SanableMain(gpr460::System* system)
{
    // Init
    EditorApplication* editor = new EditorApplication(system->GetBaseDir()/"testProject"); // TEMP TESTING HACK
    MemoryRoot::get()->registerExternal(&editor, ExternalObjectOptions::DefaultExternal);
    editor->init(*system);

    // Setup splash window
    SplashWindow* loaderWindow = new SplashWindow(editor, L"Sanable Editor - Loading...", { 300, 200 });
    editor->setMainWindow(loaderWindow->getWindow());
    system->pumpEvents();
    loaderWindow->redraw();

    // Dummy loading logic (placeholder for later)
    std::this_thread::sleep_for(2s);

    // Setup editor window
    {
        WindowSettings mainWindowSettings("Sanable Editor", 800, 600);
        GUIWindowDispatcher* editorWindowLogic = new GUIWindowDispatcher(editor, 5);
        mainWindowSettings.userLogic = editorWindowLogic;
        mainWindowSettings.resizable = true;

        Window* editorWindow = editor->buildWindow(mainWindowSettings);
        editor->setupDefaultLayout(editorWindow, &editorWindowLogic->hud);
        editor->setMainWindow(editorWindow);
    }

    // Teardown loader window and UI resources
    //delete loaderWindow;

    return editor; // System will call game->doMainLoop and game->cleanup
}
