#pragma once

#include <string>
#include <memory>
#include <SDL_video.h>
#include <SDL_events.h>
#include <EngineCoreReflectionHooks.hpp>
#include "dllapi.h"
#include "Renderer.hpp"
#include "WindowRenderPipeline.hpp"

struct SDL_Window;
struct SDL_Renderer;
class WindowBuilder;
class WindowInputProcessor;
struct GLSettings;

class Window
{
	SANABLE_REFLECTION_HOOKS

private:
	SDL_Window* handle;
	SDL_GLContext context;
	Renderer _interface;
	std::unique_ptr<WindowRenderPipeline> renderPipeline;
	std::unique_ptr<WindowInputProcessor> inputProcessor;

	EngineCore* engine;
	friend class EngineCore;
	void draw() const;
	void handleEvent(SDL_Event& ev) const;

	Window(const std::string& name, int width, int height, const GLSettings& glSettings, EngineCore* engine, std::unique_ptr<WindowRenderPipeline>&& renderPipeline, std::unique_ptr<WindowInputProcessor>&& inputProcessor);
	friend class WindowBuilder;
public:
	ENGINEGRAPHICS_API ~Window();

	ENGINEGRAPHICS_API void move(int x, int y);

	inline Renderer* getRenderer() { return &_interface; }

	ENGINEGRAPHICS_API int getWidth() const;
	ENGINEGRAPHICS_API int getHeight() const;
	ENGINEGRAPHICS_API bool isFocused() const;

	ENGINEGRAPHICS_API static void setActiveDrawTarget(const Window* w);
};
