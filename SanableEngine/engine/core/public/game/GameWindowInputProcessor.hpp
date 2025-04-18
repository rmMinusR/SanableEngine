#pragma once

#include "WindowInputProcessor.hpp"
#include "dllapi.h"

class Game;

class GameWindowInputProcessor : public WindowInputProcessor
{
	Game* game;

public:
	ENGINECORE_API GameWindowInputProcessor(Game* game);
	virtual ~GameWindowInputProcessor();

protected:
	virtual void handleEvent(SDL_Event& ev) override;
};
