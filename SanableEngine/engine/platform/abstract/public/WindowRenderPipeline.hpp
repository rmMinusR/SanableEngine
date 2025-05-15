#pragma once

#include "math/Rect.inl"

class Window;
class Application;

class WindowRenderPipeline
{
public:
	WindowRenderPipeline();
	virtual ~WindowRenderPipeline();

protected:
	Window* window;
	virtual void setup(Window* window);
	friend class Application;

	virtual void render(Rect<float> viewport) = 0;
	friend class Window;
};
