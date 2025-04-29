#include "Window.hpp"

#include "System.hpp"
#include "WindowSettings.hpp"
#include "WindowInputProcessor.hpp"
#include "WindowRenderPipeline.hpp"

void Window::handleEvent(SDL_Event& ev)
{
	if (inputProcessor) inputProcessor->handleEvent(ev);
}

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

void Window::setRenderPipeline(WindowRenderPipeline* v)
{
	if (renderPipeline) delete renderPipeline;
	renderPipeline = v;
}

void Window::setInputProcessor(WindowInputProcessor* v)
{
	if (inputProcessor) delete inputProcessor;
	inputProcessor = v;
}

bool Window::isFocused() const
{
	return system->isFocused(this);
}

void Window::draw() const
{
	if (renderPipeline)
	{
		renderPipeline->render({ Vector2f(0,0), (Vector2f)getSize() });
	}
}
