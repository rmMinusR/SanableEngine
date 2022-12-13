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

void EngineCore::applyConcurrencyBuffers()
{
    for (Component* c : componentDelBuffer) destroyImmediate(c);
    componentDelBuffer.clear();

    for (auto i : componentAddBuffer) i.first->BindComponent(i.second);
    componentAddBuffer.clear();

    for (GameObject* go : objectDelBuffer) destroyImmediate(go);
    objectDelBuffer.clear();

    for (GameObject* go : objectAddBuffer)
    {
        objects.push_back(go);
        go->InvokeStart();
    }
    objectAddBuffer.clear();
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
            if (event.key.keysym.sym == SDLK_F5) reloadPlugins();
        }
    }
}

void EngineCore::reloadPlugins()
{
    std::cout << "Hot Reload Started\n";

    std::cout << "Unloading plugins...\n";
    pluginManager.unloadAll();

    std::cout << "Loading plugins...\n";
    pluginManager.discoverAll(system.GetBaseDir() / "plugins", this);

    std::cout << "Refreshing pointers...\n";
    refreshCallBatchers();

    std::cout << "Hot Reload Complete\n";
}

void EngineCore::refreshCallBatchers()
{
    updateList.clear();
    renderList.clear();

    for (GameObject* go : objects)
    {
        for (Component* c : go->components)
        {
            IUpdatable* u = dynamic_cast<IUpdatable*>(c);
            if (u) updateList.add(u);

            IRenderable* r = dynamic_cast<IRenderable*>(c);
            if (r) renderList.add(r);
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

    pluginManager.discoverAll(system.GetBaseDir()/"plugins", this);

    if (userInitCallback) (*userInitCallback)(this);
}

void EngineCore::shutdown()
{
    assert(isAlive);
    isAlive = false;

    pluginManager.unloadAll();

    for (GameObject* o : objects) MemoryManager::destroy(o);
    //for (GameObject* o : objects) delete o;
    objects.clear();

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
    objectAddBuffer.push_back(o);
    return o;
}

void EngineCore::destroy(GameObject* go)
{
    objectDelBuffer.push_back(go);
}

void EngineCore::destroyImmediate(GameObject* go)
{
    assert(std::find(objects.cbegin(), objects.cend(), go) != objects.cend());
    objects.erase(std::find(objects.begin(), objects.end(), go));
    MemoryManager::destroy(go);
}

void EngineCore::destroyImmediate(Component* c)
{
    assert(std::find(objects.cbegin(), objects.cend(), c->getGameObject()) != objects.cend());
    auto& l = c->getGameObject()->components;
    assert(std::find(l.cbegin(), l.cend(), c) != l.cend());
    l.erase(std::find(l.begin(), l.end(), c));
    MemoryManager::destroy(c);
}

void EngineCore::doMainLoop()
{
    system.DoMainLoop();
}

void EngineCore::frameStep(void* arg)
{
    EngineCore* engine = (EngineCore*)arg;

    engine->tick();
    engine->draw();
}

void EngineCore::tick()
{
    assert(isAlive);

    frame++;
    frameStart = GetTicks();

    processEvents();

    applyConcurrencyBuffers();

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
