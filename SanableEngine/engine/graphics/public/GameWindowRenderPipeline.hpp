#pragma once

#include <EngineCoreReflectionHooks.hpp>

#include "WindowRenderPipeline.hpp"

class EngineCore;

class GameWindowRenderPipeline : public WindowRenderPipeline
{
	SANABLE_REFLECTION_HOOKS
	GameWindowRenderPipeline();

	EngineCore* engine;

public:
	ENGINEGRAPHICS_API GameWindowRenderPipeline(EngineCore* engine);
	ENGINEGRAPHICS_API virtual ~GameWindowRenderPipeline();

protected:
	ENGINEGRAPHICS_API virtual void setup(Window* window) override;
	ENGINEGRAPHICS_API virtual void render(Rect<float> viewport) override;
};
