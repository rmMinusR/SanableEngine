#include "gui/WindowGUIInputProcessor.hpp"

#include "gui/HUD.hpp"

WindowGUIInputProcessor::WindowGUIInputProcessor(HUD* hud) :
	hud(hud)
{
}

WindowGUIInputProcessor::~WindowGUIInputProcessor()
{
}

void WindowGUIInputProcessor::setPassthrough(std::unique_ptr<WindowInputProcessor>&& newPassthrough)
{
	passthrough = std::move(newPassthrough);
}
