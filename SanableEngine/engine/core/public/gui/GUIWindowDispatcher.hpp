#pragma once

#include <memory>

#include "dllapi.h"
#include "HUD.hpp"
#include "WindowUserLogic.hpp"

class HUD;
class WidgetTransform;
class Widget;

class GUIWindowDispatcher : public WindowUserLogic
{
	std::unique_ptr<WindowUserLogic> passthrough; //Input will be passed through if no GUI element consumes it

	WidgetTransform* currentlyHovered;
	Widget* currentlyHoveredWidget;

	Vector2f mouseDownPos;
	WidgetTransform* mouseDownTransform;
	Widget* mouseDownWidget;
	float minDragDistance;
	bool beingDragged;

	Vector2f lastKnownMousePos;

public:
	ENGINEGUI_API GUIWindowDispatcher(Application* application, float minDragDistance);
	ENGINEGUI_API virtual ~GUIWindowDispatcher();

	ENGINEGUI_API virtual void handleEvent(SDL_Event& ev) override;

	ENGINEGUI_API void setEventPassthrough(std::unique_ptr<WindowUserLogic>&& newPassthrough);


	HUD hud;
	float zNear = 0;
	float zFar = 1000;

protected:
	ENGINEGUI_API virtual void render(Rect<float> viewport) override;
};
