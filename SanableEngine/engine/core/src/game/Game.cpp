#include "Game.hpp"

#include "InputSystem.hpp"

Game::Game() :
    inputSystem(nullptr)
{
    delete inputSystem;
}

Game::~Game()
{
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
    GameObject* o = memoryManager.create<GameObject>(this);
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
    memoryManager.destroy(go);
}

void Game::destroyImmediate(Component* c)
{
    assert(std::find(objects.cbegin(), objects.cend(), c->getGameObject()) != objects.cend());
    auto& l = c->getGameObject()->components;
    auto it = std::find(l.cbegin(), l.cend(), c);
    assert(it != l.cend());
    l.erase(it);
    memoryManager.destroy(c);
}

void Game::tick()
{
    assert(isAlive);

    frame++;

    processEvents();

    applyConcurrencyBuffers();

    updateList.memberCall(&IUpdatable::Update);
}

void Game::draw()
{
    assert(isAlive);

    for (Window* w : windows) w->draw();
}

InputSystem* Game::getGameInput()
{
    return inputSystem;
}

const CallBatcher<I3DRenderable>* Game::get3DRenderables()
{
    return &_3dRenderList;
}
