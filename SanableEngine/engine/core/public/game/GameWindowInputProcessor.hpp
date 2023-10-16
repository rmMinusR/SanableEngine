#pragma once

#include "EngineCoreReflectionHooks.hpp"

#include "application/WindowInputProcessor.hpp"

class Game;

class GameWindowInputProcessor : public WindowInputProcessor
{
	SANABLE_REFLECTION_HOOKS
	GameWindowInputProcessor();

	Game* game;

public:
	ENGINECORE_API GameWindowInputProcessor(Game* game);
	ENGINECORE_API virtual ~GameWindowInputProcessor();

protected:
	ENGINECORE_API virtual void handleEvent(SDL_Event& ev) override;
};
