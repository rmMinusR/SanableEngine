#pragma once

#include <SDL_render.h>

#include "dllapi.h"

struct SDL_Window;
struct SDL_Renderer;

class Renderer
{
private:
	SDL_Renderer* handle;

public:
	ENGINEGRAPHICS_API Renderer();
	ENGINEGRAPHICS_API Renderer(SDL_Renderer*);

	ENGINEGRAPHICS_API void beginFrame();
	ENGINEGRAPHICS_API void finishFrame();

	ENGINEGRAPHICS_API void drawRect(const SDL_Rect& rect, const SDL_Color& color);
};
