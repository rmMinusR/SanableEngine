#include "application/WindowBuilder.hpp"

#include "application/Window.hpp"
#include "application/Application.hpp"

WindowBuilder::WindowBuilder(Application* engine, const std::string& name, int width, int height, const GLSettings& glSettings, WindowRenderPipeline* renderPipeline) :
	glSettings(glSettings),
	engine(engine),
	name(name),
	size(width, height, 0),
	renderPipeline(renderPipeline),
	inputProcessor(nullptr),
	contextSharedWith(nullptr)
{
}

WindowBuilder::~WindowBuilder()
{
	if (renderPipeline)
	{
		delete renderPipeline;
		renderPipeline = nullptr;
	}

	if (inputProcessor)
	{
		delete inputProcessor;
		inputProcessor = nullptr;
	}
}

void WindowBuilder::setInputProcessor(WindowInputProcessor* inputProcessor)
{
	if (this->inputProcessor) delete this->inputProcessor;

	this->inputProcessor = inputProcessor;
}

void WindowBuilder::shareContextWith(Window* window)
{
	contextSharedWith = window;
}

Window* WindowBuilder::build()
{
	//Setup context sharing
	//TODO move to platform abstraction layer
	if (contextSharedWith)
	{
		Window::setActiveDrawTarget(contextSharedWith);
		SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	} else SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);

	Window* window = new Window(name, size.x, size.y, glSettings, engine, renderPipeline, inputProcessor);
	if (position.has_value()) window->move(position.value().x, position.value().y);
	engine->windows.push_back(window);
	window->renderPipeline->setup(window);
	if (window->inputProcessor) window->inputProcessor->setup(window);

	renderPipeline = nullptr;
	inputProcessor = nullptr;

	return window;
}
