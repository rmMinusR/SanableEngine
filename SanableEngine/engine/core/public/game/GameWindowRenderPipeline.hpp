#pragma once

#include "application/WindowRenderPipeline.hpp"

class Game;

class GameWindowRenderPipeline : public WindowRenderPipeline
{
	Game* game;

public:
	ENGINECORE_API GameWindowRenderPipeline(Game* game);
	ENGINECORE_API virtual ~GameWindowRenderPipeline();

protected:
	ENGINECORE_API virtual void setup(Window* window) override;
	ENGINECORE_API virtual void render(Rect<float> viewport) override;
};
