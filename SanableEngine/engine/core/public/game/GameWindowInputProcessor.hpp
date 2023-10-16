#pragma once

#include "application/WindowInputProcessor.hpp"

class GameWindowInputProcessor : public WindowInputProcessor
{
public:
	ENGINECORE_API GameWindowInputProcessor();
	ENGINECORE_API virtual ~GameWindowInputProcessor();

protected:
	ENGINECORE_API virtual void handleEvent(SDL_Event& ev);
};
