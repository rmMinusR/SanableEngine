#include "application/Application.hpp"

#include <cassert>
#include <iostream>

#include <SDL.h>

#include "System.hpp"
#include "GlobalTypeRegistry.hpp"
#include "application/Window.hpp"
#include "game/Game.hpp"
#include "Camera.hpp"

void Application::processEvents()
{
    assert(isAlive);
    
    SDL_Event event;

    //Utility functions
    auto forwardToWindow = [&](SDL_Window* windowHandle)
    {
        auto it = std::find_if(windows.begin(), windows.end(), [=](Window* w) { return w->handle == windowHandle; });
        if (it != windows.end()) (*it)->handleEvent(event);
    };

    while (SDL_PollEvent(&event))
    {
        //Old testing stuff, should prob be refactored
        if (event.type == SDL_QUIT) quit = true;

        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
            if (event.key.keysym.sym == SDLK_F5) pluginManager.reloadAll();
        }

        //Foward events to appropriate windows
        switch (event.type)
        {
        case SDL_WINDOWEVENT:
            forwardToWindow(SDL_GetWindowFromID(event.window.windowID));
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
            forwardToWindow(SDL_GetKeyboardFocus());
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEMOTION:
            forwardToWindow(SDL_GetMouseFocus());
            break;

        }
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

void Application::init(Game* game, const GLSettings& glSettings, WindowBuilder& mainWindowBuilder, gpr460::System& _system, UserInitFunc userInitCallback)
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

    memoryManager.init();
    memoryManager.getSpecificPool<GameObject>(true); //Force create GameObject pool now so it's owned by main module (avoiding nasty access violation errors)
    memoryManager.getSpecificPool<Camera>(true); //Same with Camera
    memoryManager.ensureFresh();

    this->game = game;
    game->init(this);

    this->glSettings = glSettings;
    mainWindow = mainWindowBuilder.build();

    pluginManager.discoverAll(system->GetBaseDir()/"plugins");
    pluginManager.loadAll();
    pluginManager.hookAll();

    if (userInitCallback) (*userInitCallback)(this);
}

void Application::shutdown()
{
    assert(isAlive);
    isAlive = false;

    pluginManager.unhookAll(true);
    pluginManager.unloadAll();

    game->cleanup();

    while (!windows.empty()) WindowBuilder::destroy(windows[windows.size()-1]);
    mainWindow = nullptr;
    
    //Clean up memory, GameObject pool first so components are released
    memoryManager.destroyPool<GameObject>();
    memoryManager.cleanup();

    //Type info shouldn't appear on the straggling allocations report
    GlobalTypeRegistry::clear();

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

    engine->processEvents();
    engine->game->tick();
    for (Window* w : engine->windows) w->draw();
}

Game* Application::getGame() const
{
    return game;
}

gpr460::System* Application::getSystem()
{
    return system;
}

MemoryManager* Application::getMemoryManager()
{
    return &memoryManager;
}

StackAllocator* Application::getFrameAllocator()
{
    return &frameAllocator;
}

WindowBuilder Application::buildWindow(const std::string& name, int width, int height, std::unique_ptr<WindowRenderPipeline>&& renderPipeline)
{
    return WindowBuilder(this, name, width, height, glSettings, std::move(renderPipeline));
}
