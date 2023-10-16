#include "GameWindowInputProcessor.hpp"

#include "Window.hpp"
#include "Application.hpp"

GameWindowInputProcessor::GameWindowInputProcessor()
{
}

GameWindowInputProcessor::~GameWindowInputProcessor()
{
}

void GameWindowInputProcessor::handleEvent(SDL_Event& ev)
{
	window->getEngine()->getGameInput()->handleEvent(ev);
}
