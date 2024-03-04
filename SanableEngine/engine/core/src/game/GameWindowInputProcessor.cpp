#include "game/GameWindowInputProcessor.hpp"

#include "application/Application.hpp"
#include "game/Game.hpp"
#include "game/InputSystem.hpp"

GameWindowInputProcessor::GameWindowInputProcessor(Game* game) :
	game(game)
{
}

GameWindowInputProcessor::~GameWindowInputProcessor()
{
}

void GameWindowInputProcessor::handleEvent(SDL_Event& ev)
{
	if (ev.type == SDL_WINDOWEVENT)
	{
		switch (ev.window.event)
		{
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			game->getInput()->onGainFocus();
			break;

		case SDL_WINDOWEVENT_FOCUS_LOST:
			game->getInput()->onLoseFocus();
			break;
		}
	}

	//game->getInput()->handleEvent(ev);
}
