#include "game/Game.hpp"

#include <cassert>

#include "game/GameObject.hpp"
#include "game/Component.hpp"
#include "game/InputSystem.hpp"

Game::Game() :
    application(nullptr),
    inputSystem(nullptr),
    isAlive(false),
    updateList   (TypeName::create<IUpdatable   >()),
    _3dRenderList(TypeName::create<I3DRenderable>())
{
}

Game::~Game()
{
    assert(!isAlive);
}

void Game::init(Application* application)
{
    assert(!isAlive);
    isAlive = true;

    this->application = application;
    frame = 0;

    this->inputSystem = new InputSystem();
}

void Game::cleanup()
{
    assert(isAlive);
    isAlive = false;

    //Delete objects
    applyConcurrencyBuffers();
    for (GameObject* o : objects) destroy(o);
    applyConcurrencyBuffers();

    //Delete vector backing memory so it doesn't falsely appear as leaked
    objects           .clear(); objects           .shrink_to_fit();
    componentAddBuffer.clear(); componentAddBuffer.shrink_to_fit();
    componentDelBuffer.clear(); componentDelBuffer.shrink_to_fit();
    objectAddBuffer   .clear(); objectAddBuffer   .shrink_to_fit();
    objectDelBuffer   .clear(); objectDelBuffer   .shrink_to_fit();
    
    delete inputSystem;
}

void Game::applyConcurrencyBuffers()
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

void Game::refreshCallBatchers(bool force)
{
    updateList   .ensureFresh(application->getMemoryManager(), force);
    _3dRenderList.ensureFresh(application->getMemoryManager(), force);
}

GameObject* Game::addGameObject()
{
    GameObject* o = application->getMemoryManager()->create<GameObject>(this);
    objectAddBuffer.push_back(o);
    return o;
}

void Game::destroy(GameObject* go)
{
    objectDelBuffer.push_back(go);
}

void Game::destroyImmediate(GameObject* go)
{
    assert(std::find(objects.cbegin(), objects.cend(), go) != objects.cend());
    objects.erase(std::find(objects.begin(), objects.end(), go));
    application->getMemoryManager()->destroy(go);
}

void Game::destroyImmediate(Component* c)
{
    assert(std::find(objects.cbegin(), objects.cend(), c->getGameObject()) != objects.cend());
    auto& l = c->getGameObject()->components;
    auto it = std::find(l.cbegin(), l.cend(), c);
    assert(it != l.cend());
    l.erase(it);
    application->getMemoryManager()->destroy(c);
}

void Game::tick()
{
    assert(isAlive);

    frame++;

    applyConcurrencyBuffers();
    inputSystem->onTick();
    updateList.memberCall(&IUpdatable::Update);
}

InputSystem* Game::getInput()
{
    return inputSystem;
}

const PoolCallBatcher* Game::get3DRenderables() const
{
    return &_3dRenderList;
}
