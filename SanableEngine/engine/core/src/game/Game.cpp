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

    level.emplace(this);
}

void Game::cleanup()
{
    assert(isAlive);
    isAlive = false;

    level.reset();

    delete inputSystem;
}

void Game::applyConcurrencyBuffers()
{
    if (level.has_value()) level.value().applyConcurrencyBuffers();
}

void Game::refreshCallBatchers(bool force)
{
    if (level.has_value()) level.value().refreshCallBatchers(force);
}

void Game::tick()
{
    assert(isAlive);

    frame++;
    level.value().tick();
}

InputSystem* Game::getInput()
{
    return inputSystem;
}

Application* Game::getApplication() const
{
    return application;
}

void Game::visitLevels(const std::function<void(Level*)>& visitor)
{
    visitor(&level.value());
}

Level* Game::getLevel(size_t which)
{
    assert(which == 0);
    return &level.value();
}

size_t Game::getLevelCount() const
{
    return level.has_value() ? 1 : 0;
}
