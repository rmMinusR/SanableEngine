#include "game/Game.hpp"

#include <cassert>

#include "MemoryRoot.hpp"
#include "System.hpp"
#include "Window.hpp"
#include "game/GameObject.hpp"
#include "game/Component.hpp"
#include "game/InputSystem.hpp"
#include "game/Level.hpp"

Game::Game() :
    Application(),
    inputSystem(nullptr)
{
}

Game::~Game()
{
}

void Game::init(gpr460::System& system)
{
    Application::init(system);

    frameAllocator.resize(frameAllocatorSize);
    levels = heap.emplace().getSpecificPool<Level>(true);

    frame = 0;

    this->inputSystem = new InputSystem();

    refreshCallBatchers();
    heap.value().ensureFresh();
}

void Game::cleanup()
{
    //application->getHeap()->destroyPool<Level>(); //Don't do this, it throws incorrect warnings
    for (auto it = levels->cbegin(); it != levels->cend(); ++it) heap.value().destroy(&*it);
    levels = nullptr;

    delete inputSystem;

    applyConcurrencyBuffers();

    Application::cleanup();
}

void Game::applyConcurrencyBuffers()
{
    for (auto it = levels->cbegin(); it != levels->cend(); ++it)
    {
        it->applyConcurrencyBuffers();
    }
}

void Game::refreshCallBatchers(bool force)
{
    for (auto it = levels->cbegin(); it != levels->cend(); ++it)
    {
        it->refreshCallBatchers(force);
    }
}

void Game::tick()
{
    assert(isAlive);

    frame++;
    
    refreshCallBatchers();
    for (auto it = levels->cbegin(); it != levels->cend(); ++it)
    {
        it->tick();
    }
}

InputSystem* Game::getInput()
{
    return inputSystem;
}

StackAllocator* Game::getFrameAllocator()
{
    return &frameAllocator;
}

void Game::visitLevels(const std::function<void(Level*)>& visitor)
{
    for (auto it = levels->cbegin(); it != levels->cend(); ++it)
    {
        visitor(&*it);
    }
}

Level* Game::getLevel(size_t which)
{
    return &*(levels->cbegin()+which);
}

size_t Game::getLevelCount() const
{
    return levels->asGeneric()->getNumAllocatedObjects();
}

Level* Game::addLevel()
{
    return levels->emplace(this);
}

void Game::removeLevel(Level* level)
{
    levels->release(level);
}

void Game::doMainLoop()
{
    // Ensure up to date
    pluginManager.executeCommandBuffer();
    refreshCallBatchers(true);

    // Run
    system->DoMainLoop(+[](Application* arg) { static_cast<Game*>(arg)->frameStep(); }, (Application*)this);
}

void Game::frameStep()
{
    frameAllocator.restoreCheckpoint(StackAllocator::Checkpoint());

    refreshCallBatchers(false);
    system->pumpEvents();
    refreshCallBatchers(false);
    tick();
    refreshCallBatchers(false);
    for (size_t i = 0; i < system->getNumWindows(); ++i) system->getWindow(i)->draw();

    if (pluginManager.executeCommandBuffer() != 0)
    {
        MemoryRoot::get()->ensureFresh();
    }
}
