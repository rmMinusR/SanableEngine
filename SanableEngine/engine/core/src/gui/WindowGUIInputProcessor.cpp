#include "gui/WindowGUIInputProcessor.hpp"

#include "Vector2.inl"
#include "gui/HUD.hpp"
#include "application/Window.hpp"

WindowGUIInputProcessor::WindowGUIInputProcessor(HUD* hud) :
	hud(hud)
{
}

WindowGUIInputProcessor::~WindowGUIInputProcessor()
{
}

void WindowGUIInputProcessor::handleEvent(SDL_Event& ev)
{
	WindowInputProcessor::handleEvent(ev);

	bool consumed = false;
	switch (ev.type)
	{
		case SDL_EventType::SDL_MOUSEBUTTONDOWN:
		{
			Vector2f mousePos = getMousePos();
			hud->raycast(mousePos, [&](Widget* w) {
				if (!consumed && w->onMouseDown(mousePos)) consumed = true;
			});
			break;
		}

		case SDL_EventType::SDL_MOUSEBUTTONUP:
		{
			Vector2f mousePos = getMousePos();
			hud->raycast(mousePos, [&](Widget* w) {
				if (!consumed && w->onMouseUp(mousePos)) consumed = true;
			});
			break;
		}
	}
}

void WindowGUIInputProcessor::setPassthrough(std::unique_ptr<WindowInputProcessor>&& newPassthrough)
{
	passthrough = std::move(newPassthrough);
}
