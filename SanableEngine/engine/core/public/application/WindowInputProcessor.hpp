#pragma once

#include "../dllapi.h"

#include <SDL_events.h>

class Window;

class WindowInputProcessor
{
public:
	ENGINECORE_API WindowInputProcessor();
	ENGINECORE_API virtual ~WindowInputProcessor();

protected:
	Window* window;
	ENGINECORE_API virtual void setup(Window* window);

	virtual void handleEvent(SDL_Event& ev) = 0;
	friend class Window;
};
