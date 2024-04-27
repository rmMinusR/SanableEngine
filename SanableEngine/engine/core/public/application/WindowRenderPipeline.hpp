#pragma once

#include "../dllapi.h"
#include "math/Rect.inl"

class Window;
class WindowBuilder;
class Application;

class WindowRenderPipeline
{
public:
	ENGINECORE_API WindowRenderPipeline();
	ENGINECORE_API virtual ~WindowRenderPipeline();

protected:
	Window* window;
	ENGINECORE_API virtual void setup(Window* window);
	virtual void render(Rect<float> viewport) = 0;
	friend class Window;
	friend class WindowBuilder;
};
