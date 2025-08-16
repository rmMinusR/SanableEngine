#include <thread>
#include <chrono>
using namespace std::chrono_literals;

#include "MemoryRoot.hpp"
#include "System.hpp"
#include "Window.hpp"
#include "EditorApplication.hpp"
#include "SplashWindow.hpp"
#include "gui/GUIWindowDispatcher.hpp"

void SanableMain(gpr460::System* system)
{
    // Init
    EditorApplication editor(system->GetBaseDir()/"testProject"); // TEMP TESTING HACK
    MemoryRoot::get()->registerExternal(&editor, ExternalObjectOptions::DefaultExternal);
    editor.init(*system);

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
        editor.setupDefaultLayout(editorWindow, &editorWindowLogic->hud);
        editor.setMainWindow(editorWindow);
    }

    // Teardown loader window and UI resources
    //delete loaderWindow;

    editor.doMainLoop();

    // Shutdown
    editor.cleanup();
}
