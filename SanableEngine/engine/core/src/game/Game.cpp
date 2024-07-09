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
    level.value().applyConcurrencyBuffers();
}

void Game::refreshCallBatchers(bool force)
{
    level.value().refreshCallBatchers(force);
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

void Game::visitLevels(const std::function<void(Level*)>& visitor)
{
    visitor(&level.value());
}
