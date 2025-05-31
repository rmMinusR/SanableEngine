#include "application/Application.hpp"

#include <cassert>
#include <iostream>

#include <SDL.h>

#include "GlobalTypeRegistry.hpp"
#include "System.hpp"
#include "Window.hpp"
#include "WindowUserLogic.hpp"
#include "game/Game.hpp"
#include "MemoryRoot.hpp"

void Application::processEvent(SDL_Event& event)
{
    //Old testing stuff, should prob be refactored
    if (event.type == SDL_QUIT) quit = true;

    if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
        if (event.key.keysym.sym == SDLK_F5) pluginManager.reloadAll();
    }
}

Application::Application() :
    isAlive(false),
    system(nullptr),
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

void Application::init(Game* game, WindowSettings& mainWindowSettings, gpr460::System& _system)
{
    assert(!isAlive);
    isAlive = true;
    quit = false;

    frameAllocator.resize(frameAllocatorSize);

    this->system = &_system;
    system->Init(this);

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

    heap.emplace().getSpecificPool<Level>(true);

    this->game = game;
    game->init(this);

    mainWindow = buildWindow(mainWindowSettings);

    pluginManager.discoverAll(system->GetBaseDir()/"plugins");
    std::cout << "Discovered " << pluginManager.plugins.size() << " plugins" << std::endl;
    for (Plugin const* p : pluginManager.plugins) std::cout << " - " << std::filesystem::relative( p->getPluginDir(), system->GetBaseDir() ).string() << std::endl;
    pluginManager.loadAll();
    pluginManager.hookAll();

    heap.value().ensureFresh();
    game->refreshCallBatchers();
}

void Application::shutdown()
{
    assert(isAlive);
    isAlive = false;

    game->applyConcurrencyBuffers();
    pluginManager.unhookAll(true); //FIXME: Pools destroyed automatically here, but Component and GameObject need to interface with Game
    game->applyConcurrencyBuffers();
    game->cleanup();

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
    system->Shutdown();
}

void Application::doMainLoop()
{
    system->DoMainLoop();
}

void Application::frameStep(void* arg)
{
    Application* engine = (Application*)arg;

    engine->frameAllocator.restoreCheckpoint(StackAllocator::Checkpoint());

    engine->game->refreshCallBatchers(false);
    engine->system->pumpEvents();
    engine->game->refreshCallBatchers(false);
    engine->game->tick();
    engine->game->refreshCallBatchers(false);
    for (size_t i = 0; i < engine->system->getNumWindows(); ++i) engine->system->getWindow(i)->draw();

    if (engine->pluginManager.executeCommandBuffer() != 0)
    {
        MemoryRoot::get()->ensureFresh();
    }
}

Game* Application::getGame() const
{
    return game;
}

gpr460::System* Application::getSystem()
{
    return system;
}

MemoryHeap* Application::getHeap()
{
    return &heap.value();
}

StackAllocator* Application::getFrameAllocator()
{
    return &frameAllocator;
}

PluginManager* Application::getPluginManager()
{
    return &pluginManager;
}

Window* Application::getMainWindow()
{
    return mainWindow;
}

Window* Application::buildWindow(WindowSettings& settings)
{
    Window* window = system->createWindow(settings, this);
	if (settings.position.has_value()) window->move(settings.position.value().x, settings.position.value().y);
	window->userLogic->setup(window);

    settings.userLogic = nullptr;

	return window;
}
