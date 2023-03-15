#include "EngineCore.hpp"

#include <cassert>
#include <iostream>

#include <SDL.h>

#include "SDLModule.hpp"

#include "GameObject.hpp"
#include "Component.hpp"
#include "System.hpp"

void EngineCore::applyConcurrencyBuffers()
{
    for (Component* c : componentDelBuffer) destroyImmediate(c);
    componentDelBuffer.clear();

    for (GameObject* go : objectDelBuffer) destroyImmediate(go);
    objectDelBuffer.clear();

    for (GameObject* go : objectAddBuffer)
    {
        objects.push_back(go);
        go->InvokeStart();
    }
    objectAddBuffer.clear();

    for (auto& i : componentAddBuffer) i.second->BindComponent(i.first);
    for (auto& i : componentAddBuffer) i.first->onStart();
    componentAddBuffer.clear();
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
            if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
            if (event.key.keysym.sym == SDLK_F5) pluginManager.reloadAll();
        }
    }
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
    system(nullptr),
    mainWindow(nullptr),
    pluginManager(this)
{
}

EngineCore::~EngineCore()
{
    assert(!isAlive);
}

void EngineCore::init(char const* windowName, int windowWidth, int windowHeight, gpr460::System& _system, UserInitFunc userInitCallback)
{
    assert(!isAlive);
    isAlive = true;
    quit = false;

    this->system = &_system;
    system->Init(this);
    memoryManager.init();
    memoryManager.getSpecificPool<GameObject>(true); //Force create GameObject pool now so it's owned by main module

    SDLModule::video.load(&memoryManager);

    mainWindow = new Window(windowName, windowWidth, windowHeight);
    frame = 0;

    pluginManager.discoverAll(system->GetBaseDir()/"plugins");

    if (userInitCallback) (*userInitCallback)(this);
}

void EngineCore::shutdown()
{
    assert(isAlive);
    isAlive = false;

    for (GameObject* o : objects) memoryManager.destroy(o);
    objects.clear();

    pluginManager.unhookAll(true);
    pluginManager.unloadAll();

    delete mainWindow;
    mainWindow = nullptr;
    
    SDLModule::video.unload(&memoryManager);

    //Clean up memory, GameObject pool first so components are released
    memoryManager.destroyPool<GameObject>();
    memoryManager.cleanup();

    system->Shutdown();
}

GameObject* EngineCore::addGameObject()
{
    GameObject* o = memoryManager.create<GameObject>(this);
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
    memoryManager.destroy(go);
}

void EngineCore::destroyImmediate(Component* c)
{
    assert(std::find(objects.cbegin(), objects.cend(), c->getGameObject()) != objects.cend());
    auto& l = c->getGameObject()->components;
    auto it = std::find(l.cbegin(), l.cend(), c);
    assert(it != l.cend());
    l.erase(it);
    memoryManager.destroy(c);
}

void EngineCore::doMainLoop()
{
    system->DoMainLoop();
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
    
    processEvents();

    applyConcurrencyBuffers();

    updateList.memberCall(&IUpdatable::Update);
}

void EngineCore::draw()
{
    assert(isAlive);

    //Clear screen
    mainWindow->getRenderer()->beginFrame();

    //Draw objects
    renderList.memberCall(&IRenderable::Render, mainWindow->getRenderer());

    mainWindow->getRenderer()->finishFrame();
}
