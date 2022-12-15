#include "EngineCore.hpp"

#include <iostream>

#include "GameObject.hpp"
#include "Component.hpp"
#include <cassert>

EngineCore engine;

Uint32 GetTicks()
{
    return SDL_GetTicks();
}

void EngineCore::processEvents()
{
    assert(isAlive);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) quit = true;

        if (event.type == SDL_KEYDOWN)
        {
            std::cout << "Key pressed!\n";
            if (event.key.keysym.sym == SDLK_k)
            {
                std::cout << "K pressed!\n";

                // TODO: Add calls to ErrorMessage and LogToErrorFile here
            }
            if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
        }
    }
}

EngineCore::EngineCore() :
    isAlive(false),
    window(nullptr),
    renderer(nullptr)
{
}

EngineCore::~EngineCore()
{
    assert(!isAlive);
}

void EngineCore::init(char const* windowName, int windowWidth, int windowHeight, UserInitFunc userInitCallback)
{
    assert(!isAlive);
    isAlive = true;

    system.Init(this);
    MemoryManager::init();

    quit = false;
    window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    frame = 0;
    frameStart = GetTicks();

    frameAllocator = new StackAllocator(frameAllocatorSize);

    StackAllocator::Checkpoint checkpoint = frameAllocator->markCheckpoint();
    if (userInitCallback) (*userInitCallback)(this);
    frameAllocator->restoreCheckpoint(checkpoint);
}

void EngineCore::shutdown()
{
    assert(isAlive);
    isAlive = false;

    for (GameObject* o : objects) MemoryManager::destroy(o);
    //for (GameObject* o : objects) delete o;
    objects.clear();

    delete frameAllocator;

    SDL_DestroyRenderer(renderer);
    renderer = nullptr;

    SDL_DestroyWindow(window);
    window = nullptr;

    MemoryManager::cleanup();
    system.Shutdown();
}

GameObject* EngineCore::addGameObject()
{
    GameObject* o = MemoryManager::create<GameObject>();
    objects.push_back(o);
    return o;
}

void EngineCore::doMainLoop()
{
    system.DoMainLoop();
}

void EngineCore::frameStep(void* arg)
{
    EngineCore* engine = (EngineCore*)arg;

    StackAllocator::Checkpoint checkpoint = engine->frameAllocator->markCheckpoint();

    engine->tick();
    engine->draw();

    engine->frameAllocator->restoreCheckpoint(checkpoint);
}

void EngineCore::tick()
{
    assert(isAlive);

    frame++;
    frameStart = GetTicks();

    processEvents();

    updateList.memberCall(&IUpdatable::Update);
}

void EngineCore::draw()
{
    assert(isAlive);

    //Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    //Draw objects
    renderList.memberCall(&IRenderable::Render);

    SDL_RenderPresent(renderer);
}
