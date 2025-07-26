#include "EditorApplication.hpp"

#include "game/Game.hpp"
#include "System.hpp"
#include "Window.hpp"
#include "MemoryRoot.hpp"

EditorApplication::EditorApplication(gpr460::System& system) :
	Application(system)
{
}

EditorApplication::~EditorApplication()
{
}

void EditorApplication::init()
{
    Application::init();
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
    if (!currentGame) system->pumpEvents();
    else currentGame->frameStep(); // Calls pumpEvents on the System facade, which just passes through to root System

    // Other than play-in-editor, the editor is an entirely
    // GUI application, so this doubles as tick for all UI
    for (size_t i = 0; i < system->getNumWindows(); ++i) system->getWindow(i)->draw();

    if (pluginManager.executeCommandBuffer() != 0)
    {
        MemoryRoot::get()->ensureFresh();
    }
}
