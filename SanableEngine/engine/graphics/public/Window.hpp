#pragma once

#include <string>
#include <memory>
#include <SDL_video.h>
#include "dllapi.h"
#include "Renderer.hpp"
#include "WindowRenderPipeline.hpp"

struct SDL_Window;
struct SDL_Renderer;
class WindowBuilder;

class Window
{
private:
	SDL_Window* handle;
	SDL_GLContext context;
	Renderer _interface;
	std::unique_ptr<WindowRenderPipeline> renderPipeline;

	EngineCore* engine;
	friend class EngineCore;
	void draw() const;

	Window(const std::string& name, int width, int height, EngineCore* engine, std::unique_ptr<WindowRenderPipeline>&& renderPipeline);
	friend class WindowBuilder;
public:
	ENGINEGRAPHICS_API ~Window();

	ENGINEGRAPHICS_API void move(int x, int y);

	inline Renderer* getRenderer() { return &_interface; }

	ENGINEGRAPHICS_API int getWidth() const;
	ENGINEGRAPHICS_API int getHeight() const;
};
