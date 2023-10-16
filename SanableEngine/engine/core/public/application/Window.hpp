#pragma once

#include <string>
#include <memory>
#include <SDL_video.h>
#include <SDL_events.h>
#include <EngineCoreReflectionHooks.hpp>
#include "../dllapi.h"
#include "Renderer.hpp"
#include "WindowRenderPipeline.hpp"

class WindowBuilder;
class WindowInputProcessor;
struct GLSettings;

class Window
{
	SANABLE_REFLECTION_HOOKS
	Window();

private:
	SDL_Window* handle;
	SDL_GLContext context;
	Renderer _interface;
	std::unique_ptr<WindowRenderPipeline> renderPipeline;
	std::unique_ptr<WindowInputProcessor> inputProcessor;

	Application* engine;
	friend class Application;
	void draw() const;
	void handleEvent(SDL_Event& ev) const;

	Window(const std::string& name, int width, int height, const GLSettings& glSettings, Application* engine, std::unique_ptr<WindowRenderPipeline>&& renderPipeline, std::unique_ptr<WindowInputProcessor>&& inputProcessor);
	friend class WindowBuilder;
public:
	ENGINEGRAPHICS_API ~Window();

	ENGINEGRAPHICS_API void move(int x, int y);

	inline Renderer* getRenderer() { return &_interface; }
	inline Application* getEngine() { return engine; }

	ENGINEGRAPHICS_API int getWidth() const;
	ENGINEGRAPHICS_API int getHeight() const;
	ENGINEGRAPHICS_API bool isFocused() const;

	ENGINEGRAPHICS_API static void setActiveDrawTarget(const Window* w);
};
