#include "gui/GUIWindowDispatcher.hpp"

#include <SDL_events.h>

#include "math/Vector2.inl"
#include "Window.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"

GUIWindowDispatcher::GUIWindowDispatcher(Application* application, float minDragDistance) :
	hud(application),
	currentlyHovered(nullptr),
	currentlyHoveredWidget(nullptr),
	mouseDownTransform(nullptr),
	mouseDownWidget(nullptr),
	mouseDownPos(),
	minDragDistance(minDragDistance),
	beingDragged(false)
{
}

GUIWindowDispatcher::~GUIWindowDispatcher()
{
}

void GUIWindowDispatcher::setup(Window* window)
{
	WindowUserLogic::setup(window);
	hud.init(window);
}

void GUIWindowDispatcher::handleEvent(const SDL_Event& ev)
{
	if (ev.type == SDL_EventType::SDL_MOUSEMOTION)
	{
		lastKnownMousePos.set(ev.motion.x, ev.motion.y);
	}

	Vector2f mousePos = lastKnownMousePos;

	//Update currently-hovered widget
	//Only perform if event is in mouse family, or window focus
	if (ev.type == SDL_EventType::SDL_MOUSEMOTION || ev.type == SDL_EventType::SDL_WINDOWEVENT)
	{
		WidgetTransform* _newHover = hud.raycastClosest(mousePos);
		if (currentlyHovered != _newHover)
		{
			if (currentlyHoveredWidget) currentlyHoveredWidget->onMouseExit(mousePos);
			currentlyHovered = _newHover;
			currentlyHoveredWidget = currentlyHovered ? currentlyHovered->getWidget() : nullptr;
			if (currentlyHoveredWidget) currentlyHoveredWidget->onMouseEnter(mousePos);
		}
	}

	//Send mouse down/up signals
	switch (ev.type)
	{
		case SDL_EventType::SDL_MOUSEBUTTONDOWN:
		{
			mouseDownPos = mousePos;
			mouseDownTransform = currentlyHovered;
			mouseDownWidget = currentlyHoveredWidget;
			if (currentlyHoveredWidget) currentlyHoveredWidget->onMouseDown(mousePos);
			break;
		}

		case SDL_EventType::SDL_MOUSEBUTTONUP:
		{
			if (currentlyHoveredWidget)
			{
				currentlyHoveredWidget->onMouseUp(mousePos);
				if (!beingDragged && mouseDownWidget == currentlyHoveredWidget) currentlyHoveredWidget->onClicked(mousePos);
			}
			break;
		}
	}

	//Send mouse drag signals
	switch (ev.type)
	{
		case SDL_EventType::SDL_MOUSEBUTTONDOWN:
		{
			//Drag event proper doesn't start until mouse moves enough
			beingDragged = false;
			break;
		}

		case SDL_EventType::SDL_MOUSEMOTION:
		{
			//Track when a drag event starts
			if (!beingDragged)
			{
				Uint32 buttons = SDL_GetMouseState(nullptr, nullptr);
				if (buttons != 0 && (mousePos-mouseDownPos).mgn() > minDragDistance)
				{
					beingDragged = true;
					if (mouseDownWidget) mouseDownWidget->onDragStarted(mouseDownPos, mousePos);
				}
			}

			//Send while-dragged event
			if (beingDragged)
			{
				if (mouseDownWidget       ) mouseDownWidget       ->whileDragged(mouseDownPos, mouseDownWidget, mousePos, currentlyHoveredWidget);
				if (currentlyHoveredWidget) currentlyHoveredWidget->whileDragged(mouseDownPos, mouseDownWidget, mousePos, currentlyHoveredWidget);
			}
			break;
		}

		case SDL_EventType::SDL_MOUSEBUTTONUP:
		{
			if (beingDragged)
			{
				if (mouseDownWidget       ) mouseDownWidget       ->onDragFinished(mouseDownPos, mouseDownWidget, mousePos, currentlyHoveredWidget);
				if (currentlyHoveredWidget) currentlyHoveredWidget->onDragFinished(mouseDownPos, mouseDownWidget, mousePos, currentlyHoveredWidget);
				beingDragged = false;
			}
			break;
		}
	}
}

void GUIWindowDispatcher::setEventPassthrough(std::unique_ptr<WindowUserLogic>&& newPassthrough)
{
	passthrough = std::move(newPassthrough);
}

void GUIWindowDispatcher::render(Rect<float> viewport)
{
	Renderer* renderer = window->getRenderer();

	Camera cam;
	cam.setGUIProj();
	renderer->beginFrame(cam, viewport, { 0,0,0 }, glm::identity<glm::quat>(), window->getFramebuffer());

	renderer->clear({ 0, 0, 0, 1 });

	//Tick and render GUI
	hud.refreshLayout(viewport);
	hud.tick();
	hud.render(window->getRenderer());
}
