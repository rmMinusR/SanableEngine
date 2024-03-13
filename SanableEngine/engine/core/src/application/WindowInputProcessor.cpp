#include "application/WindowInputProcessor.hpp"

#include "application/Window.hpp"

WindowInputProcessor::WindowInputProcessor() :
	window(nullptr),
	lastKnownMousePos(0, 0)
{
}

WindowInputProcessor::~WindowInputProcessor()
{
}

Vector2f WindowInputProcessor::getMousePos() const
{
	if (window->isFocused())
	{
		Vector2<int> raw;
		SDL_GetMouseState(&raw.x, &raw.y);
		lastKnownMousePos.set(raw.x, raw.y);
	}

	return lastKnownMousePos;
}

void WindowInputProcessor::setup(Window* window)
{
	this->window = window;
}

void WindowInputProcessor::handleEvent(SDL_Event& ev)
{
	if (ev.type == SDL_EventType::SDL_MOUSEMOTION)
	{
		if (ev.motion.windowID == window->sdlID)
		{
			lastKnownMousePos.set(ev.motion.x, ev.motion.y);
		}
	}
}
