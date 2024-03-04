#pragma once

#include "application/WindowInputProcessor.hpp"

class Game;

class GameWindowInputProcessor : public WindowInputProcessor
{
	Game* game;

public:
	ENGINECORE_API GameWindowInputProcessor(Game* game);
	ENGINECORE_API virtual ~GameWindowInputProcessor();

protected:
	ENGINECORE_API virtual void handleEvent(SDL_Event& ev) override;
};
