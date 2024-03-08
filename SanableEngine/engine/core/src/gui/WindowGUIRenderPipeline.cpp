#include "gui/WindowGUIRenderPipeline.hpp"

#include "application/Window.hpp"

WindowGUIRenderPipeline::WindowGUIRenderPipeline()
{
}

WindowGUIRenderPipeline::~WindowGUIRenderPipeline()
{
}

void WindowGUIRenderPipeline::render(Rect<float> viewport)
{
	hud.render(window->getRenderer());
}
