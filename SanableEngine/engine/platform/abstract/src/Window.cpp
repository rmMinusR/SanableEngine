#include "Window.hpp"

#include "System.hpp"
#include "WindowSettings.hpp"

Window::Window(const WindowSettings& settings, gpr460::System* system, void* context) :
	system(system),
	context(context),
	renderPipeline(settings.renderPipeline),
	inputProcessor(settings.inputProcessor),
	closeRequested(false)
{
}

Window::~Window()
{
}

WindowRenderPipeline* Window::getRenderPipeline()
{
	return renderPipeline;
}

WindowInputProcessor* Window::getInputProcessor()
{
	return inputProcessor;
}

bool Window::isFocused() const
{
	return system->isFocused(this);
}
