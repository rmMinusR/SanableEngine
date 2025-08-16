#include "application/Application.hpp"

#include <cassert>
#include <iostream>

#include <SDL.h>

#include "GlobalTypeRegistry.hpp"
#include "System.hpp"
#include "Window.hpp"
#include "WindowUserLogic.hpp"
#include "game/Game.hpp"

Application::Application() :
    isAlive(false),
    pluginManager(this),
    mainWindow(nullptr)
{
}

Application::~Application()
{
    assert(!isAlive);
}

void engine_reportTypes(ModuleTypeRegistry* registry);
//API_IMPORT void graphics_abstract_reportTypes(ModuleTypeRegistry* registry); // TODO

void Application::init(gpr460::System& system)
{
    assert(!isAlive);
    isAlive = true;

    this->system = &system;

    //Prepare RTTI
    {
        ModuleTypeRegistry m;
        engine_reportTypes(&m);
        GlobalTypeRegistry::loadModule("Application", m);
    }
    //{
    //    ModuleTypeRegistry m;
    //    graphics_abstract_reportTypes(&m);
    //    GlobalTypeRegistry::loadModule("GraphicsAbstract", m);
    //}
}

void Application::cleanup()
{
    assert(isAlive);
    isAlive = false;

    pluginManager.unhookAll(true); //FIXME: Pools destroyed automatically here, but Component and GameObject need to interface with Game

    //If any plugins didn't clean up their window, do it for them
    while (system->getNumWindows() != 0)
    {
        Window* w = system->getWindow(system->getNumWindows() - 1);
        system->destroyWindow(w);
    }
    mainWindow = nullptr;
    
    heap.value().destroyPool<GameObject>(); //Clean up memory, GameObject pool first so remaining components are released
    pluginManager.unloadAll(); //Unload plugin code, handling destructors of globals in module

    //RTTI and plugin info shouldn't appear on the leaks report
    GlobalTypeRegistry::clear();
    pluginManager.forgetAll();

    heap.reset(); //Finish cleaning up memory
}

gpr460::System* Application::getSystem()
{
    return system;
}

MemoryHeap* Application::getHeap()
{
    return &heap.value();
}

PluginManager* Application::getPluginManager()
{
    return &pluginManager;
}

Window* Application::getMainWindow()
{
    for (size_t i = 0; i < system->getNumWindows(); ++i)
    {
        if (system->getWindow(i) == mainWindow) // Ensure both alive, and owned by this
        {
            return mainWindow;
        }
    }
    return nullptr;
}

const Window* Application::getMainWindow() const
{
    for (size_t i = 0; i < system->getNumWindows(); ++i)
    {
        if (system->getWindow(i) == mainWindow) // Ensure both alive, and owned by this
        {
            return mainWindow;
        }
    }
    return nullptr;
}

void Application::setMainWindow(Window* w)
{
    mainWindow = w;
}

Window* Application::buildWindow(WindowSettings& settings)
{
    Window* window = system->createWindow(settings, this);
	window->userLogic->setup(window);

    settings.userLogic = nullptr;

	return window;
}
