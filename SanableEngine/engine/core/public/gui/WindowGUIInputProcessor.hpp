#pragma once

#include <memory>
#include "application/WindowInputProcessor.hpp"
#include "dllapi.h"

class HUD;
class Widget;

class WindowGUIInputProcessor : public WindowInputProcessor
{
	std::unique_ptr<WindowInputProcessor> passthrough; //Input will be passed through if no GUI element consumes it

	HUD* hud;
	Widget* currentlyHovered;

	Vector2f mouseDownPos;
	Widget* mouseDownWidget;
	float minDragDistance;
	bool beingDragged;

public:
	ENGINEGUI_API WindowGUIInputProcessor(HUD* hud, float minDragDistance);
	ENGINEGUI_API ~WindowGUIInputProcessor();

	ENGINEGUI_API virtual void handleEvent(SDL_Event& ev) override;

	ENGINEGUI_API void setPassthrough(std::unique_ptr<WindowInputProcessor>&& newPassthrough);
};
