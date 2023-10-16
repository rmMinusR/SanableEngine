#pragma once

#include <EngineCoreReflectionHooks.hpp>
#include "../dllapi.h"
#include "Rect.hpp"

class Window;
class Application;

class WindowRenderPipeline
{
	SANABLE_REFLECTION_HOOKS

public:
	ENGINECORE_API WindowRenderPipeline();
	ENGINECORE_API virtual ~WindowRenderPipeline();

protected:
	Window* window;
	ENGINECORE_API virtual void setup(Window* window);
	virtual void render(Rect<float> viewport) = 0;
	friend class Window;
};
