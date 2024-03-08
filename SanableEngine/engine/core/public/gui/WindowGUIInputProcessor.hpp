#pragma once

#include <memory>
#include "application/WindowInputProcessor.hpp"
#include "dllapi.h"

class HUD;

class WindowGUIInputProcessor : public WindowInputProcessor
{
	std::unique_ptr<WindowInputProcessor> passthrough; //Input will be passed through if no GUI element consumes it

	HUD* hud;

public:
	ENGINEGUI_API WindowGUIInputProcessor(HUD* hud);
	ENGINEGUI_API ~WindowGUIInputProcessor();

	ENGINEGUI_API void setPassthrough(std::unique_ptr<WindowInputProcessor>&& newPassthrough);
};
