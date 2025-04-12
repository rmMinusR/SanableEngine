#pragma once

#include "math/Vector2.inl"

union SDL_Event;

class Window;
class Application;

class WindowInputProcessor
{
public:
	WindowInputProcessor();
	virtual ~WindowInputProcessor();

	Vector2f getMousePos() const;

protected:
	Window* window;
	virtual void setup(Window* window);
	friend class Application;

	virtual void handleEvent(SDL_Event& ev);
	friend class Window;

private:
	mutable Vector2f lastKnownMousePos;
};
