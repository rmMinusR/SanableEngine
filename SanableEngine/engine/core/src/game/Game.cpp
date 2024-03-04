#include "game/Game.hpp"

#include <cassert>

#include "game/GameObject.hpp"
#include "game/Component.hpp"
#include "game/InputSystem.hpp"

Game::Game() :
    application(nullptr),
    inputSystem(nullptr),
    isAlive(false)
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

    applyConcurrencyBuffers();
    for (GameObject* o : objects) destroy(o);
    applyConcurrencyBuffers();

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

void Game::refreshCallBatchers()
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

const CallBatcher<I3DRenderable>* Game::get3DRenderables()
{
    return &_3dRenderList;
}
