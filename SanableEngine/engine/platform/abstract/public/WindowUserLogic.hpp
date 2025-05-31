#pragma once

#include "SDL_events.h"

#include "math/Rect.inl"

class Window;
class Application;

class WindowUserLogic
{
public:
	WindowUserLogic();
	virtual ~WindowUserLogic();

protected:
	Window* window = nullptr;
	virtual void setup(Window* window);
	friend class Application;

	virtual void handleEvent(SDL_Event& ev) = 0;
	virtual void render(Rect<float> viewport) = 0;
	friend class Window;
};
