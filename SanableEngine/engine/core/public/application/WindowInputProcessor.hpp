#pragma once

#include "math/Vector2.inl"
#include "../dllapi.h"

#include <SDL_events.h>

class Window;
class Application;

class WindowInputProcessor
{
public:
	ENGINECORE_API WindowInputProcessor();
	ENGINECORE_API virtual ~WindowInputProcessor();

	ENGINECORE_API Vector2f getMousePos() const;

protected:
	Window* window;
	ENGINECORE_API virtual void setup(Window* window);
	friend class Application;

	ENGINECORE_API virtual void handleEvent(SDL_Event& ev);
	friend class Window;

private:
	mutable Vector2f lastKnownMousePos;
};
