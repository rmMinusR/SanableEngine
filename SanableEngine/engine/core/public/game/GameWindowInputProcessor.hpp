#pragma once

#include "WindowInputProcessor.hpp"

class Game;

class GameWindowInputProcessor : public WindowInputProcessor
{
	Game* game;

public:
	GameWindowInputProcessor(Game* game);
	virtual ~GameWindowInputProcessor();

protected:
	virtual void handleEvent(SDL_Event& ev) override;
};
