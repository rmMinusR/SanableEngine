#include "EngineCore.hpp"

#include <cassert>
#include <iostream>

#include <SDL.h>

#include "GameObject.hpp"
#include "Component.hpp"
#include "System.hpp"
#include "GlobalTypeRegistry.hpp"
#include "Window.hpp"
#include "WindowRenderPipeline.hpp"
#include "Camera.hpp"

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
    _3dRenderList.clear();

    for (GameObject* go : objects)
    {
        for (Component* c : go->components)
        {
            IUpdatable* u = dynamic_cast<IUpdatable*>(c);
            if (u) updateList.add(u);

            I3DRenderable* r = dynamic_cast<I3DRenderable*>(c);
            if (r) _3dRenderList.add(r);
        }
    }
}

EngineCore::EngineCore() :
    isAlive(false),
    system(nullptr),
    pluginManager(this),
    mainWindow(nullptr)
{
}

EngineCore::~EngineCore()
{
    assert(!isAlive);
}

void EngineCore::init(const GLSettings& glSettings, WindowBuilder& mainWindowBuilder, gpr460::System& _system, UserInitFunc userInitCallback)
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
        GlobalTypeRegistry::loadModule("EngineCore", m);
    }

    memoryManager.init();
    memoryManager.getSpecificPool<GameObject>(true); //Force create GameObject pool now so it's owned by main module (avoiding nasty access violation errors)
    memoryManager.getSpecificPool<Camera>(true); //Same with Camera
    memoryManager.ensureFresh();

    this->glSettings = glSettings;
    mainWindow = mainWindowBuilder.build();
    frame = 0;

    pluginManager.discoverAll(system->GetBaseDir()/"plugins");

    if (userInitCallback) (*userInitCallback)(this);
}

void EngineCore::shutdown()
{
    assert(isAlive);
    isAlive = false;

    pluginManager.unhookAll(true);
    pluginManager.unloadAll();
    applyConcurrencyBuffers();

    for (GameObject* o : objects) destroy(o);
    applyConcurrencyBuffers();

    delete mainWindow;
    mainWindow = nullptr;
    
    //Clean up memory, GameObject pool first so components are released
    memoryManager.destroyPool<GameObject>();
    memoryManager.cleanup();

    //Type info shouldn't appear on the straggling allocations report
    GlobalTypeRegistry::clear();

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

    engine->frameAllocator.restoreCheckpoint(StackAllocator::Checkpoint());
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

    for (Window* w : windows) w->draw();
}

gpr460::System* EngineCore::getSystem()
{
    return system;
}

MemoryManager* EngineCore::getMemoryManager()
{
    return &memoryManager;
}

StackAllocator* EngineCore::getFrameAllocator()
{
    return &frameAllocator;
}

const CallBatcher<I3DRenderable>* EngineCore::get3DRenderables()
{
    return &_3dRenderList;
}

Renderer* EngineCore::getRenderer()
{
    return mainWindow->getRenderer();
}

Window* EngineCore::getMainWindow()
{
    return mainWindow;
}

WindowBuilder EngineCore::buildWindow(const std::string& name, int width, int height, std::unique_ptr<WindowRenderPipeline>&& renderPipeline)
{
    return WindowBuilder(this, name, width, height, glSettings, std::move(renderPipeline));
}
