#include "gui/WindowGUIInputProcessor.hpp"

#include "math/Vector2.inl"
#include "gui/HUD.hpp"
#include "application/Window.hpp"

WindowGUIInputProcessor::WindowGUIInputProcessor(HUD* hud, float minDragDistance) :
	hud(hud),
	currentlyHovered(nullptr),
	dragStartWidget(nullptr),
	dragStartPos(),
	minDragDistance(minDragDistance),
	beingDragged(false)
{
}

WindowGUIInputProcessor::~WindowGUIInputProcessor()
{
}

void WindowGUIInputProcessor::handleEvent(SDL_Event& ev)
{
	WindowInputProcessor::handleEvent(ev);

	Vector2f mousePos = getMousePos();

	//Update currently-hovered widget
	//Only perform if event is in mouse family, or window focus
	if (ev.type == SDL_EventType::SDL_MOUSEMOTION || ev.type == SDL_EventType::SDL_WINDOWEVENT)
	{
		Widget* _newHover = hud->raycastClosest(mousePos);
		if (currentlyHovered != _newHover)
		{
			if (currentlyHovered) currentlyHovered->onMouseExit(mousePos);
			currentlyHovered = _newHover;
			if (currentlyHovered) currentlyHovered->onMouseEnter(mousePos);
		}
	}

	//Send mouse down/up signals
	switch (ev.type)
	{
		case SDL_EventType::SDL_MOUSEBUTTONDOWN: if (currentlyHovered) currentlyHovered->onMouseDown(mousePos); break;
		case SDL_EventType::SDL_MOUSEBUTTONUP  : if (currentlyHovered) currentlyHovered->onMouseUp  (mousePos); break;
	}

	//Send mouse drag signals
	switch (ev.type)
	{
		case SDL_EventType::SDL_MOUSEBUTTONDOWN:
		{
			//Drag event proper doesn't start until mouse moves enough
			beingDragged = false;
			dragStartPos = mousePos;
			dragStartWidget = currentlyHovered;
			break;
		}

		case SDL_EventType::SDL_MOUSEMOTION:
		{
			//Track when a drag event starts
			if (!beingDragged)
			{
				Uint32 buttons = SDL_GetMouseState(nullptr, nullptr);
				if (buttons != 0 && (mousePos-dragStartPos).mgn() > minDragDistance)
				{
					beingDragged = true;
					if (dragStartWidget) dragStartWidget->onDragStarted(dragStartPos, mousePos);
				}
			}

			//Send while-dragged event
			if (beingDragged) currentlyHovered->whileDragged(dragStartPos, dragStartWidget, mousePos);
			break;
		}

		case SDL_EventType::SDL_MOUSEBUTTONUP:
		{
			if (beingDragged)
			{
				if (dragStartWidget ) dragStartWidget ->onDragFinished(dragStartPos, dragStartWidget, mousePos, currentlyHovered);
				if (currentlyHovered) currentlyHovered->onDragFinished(dragStartPos, dragStartWidget, mousePos, currentlyHovered);
				beingDragged = false;
			}
			break;
		}
	}
}

void WindowGUIInputProcessor::setPassthrough(std::unique_ptr<WindowInputProcessor>&& newPassthrough)
{
	passthrough = std::move(newPassthrough);
}
