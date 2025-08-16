#pragma once

#include "dllapi.h"
#include "WindowUserLogic.hpp"
#include "gui/HUD.hpp"

class Game;

class GameWindowDispatcher : public WindowUserLogic
{
	Game* game;

public:
	ENGINECORE_API GameWindowDispatcher(Game* game);
	ENGINECORE_API virtual ~GameWindowDispatcher();

	HUD hud;

protected:
	ENGINECORE_API virtual void handleEvent(const SDL_Event& ev) override;
	ENGINECORE_API virtual void render(Rect<float> viewport) override;
};
