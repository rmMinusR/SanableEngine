#include <thread>
#include <chrono>
using namespace std::chrono_literals;

#include "System.hpp"
#include "application/Application.hpp"
#include "gui/GUIWindowDispatcher.hpp"
#include "LoaderWindow.hpp"

void SanableMain(Application* application)
{
    // Setup loader window
    LoaderWindow* loaderWindow = new LoaderWindow(application, L"Sanable Editor - Loading...", { 300, 200 });
    application->setMainWindow(loaderWindow->getWindow());
    application->getSystem()->pumpEvents();

    // Dummy loading logic (placeholder for later)
    std::this_thread::sleep_for(2s);

    // Setup editor window
    {
        WindowSettings mainWindowSettings("Sanable Editor", 800, 600);
        mainWindowSettings.userLogic = new GUIWindowDispatcher(application, 5);

        Window* gameWindow = application->buildWindow(mainWindowSettings);
        application->setMainWindow(gameWindow);
    }

    // Teardown loader window and UI resources
    delete loaderWindow;

    application->doMainLoop();
}
