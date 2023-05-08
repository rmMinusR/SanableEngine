#pragma once

#include "AssetUser.hpp"

#include "dllapi.h"
#include "Renderer.hpp"

struct SDL_Window;
struct SDL_Renderer;

class Window : public AssetUser
{
private:
	SDL_Window* handle;
	SDL_Renderer* renderer;

	Renderer _interface;

public:
	ENGINEGRAPHICS_API Window(char const* name, int width, int height);
	ENGINEGRAPHICS_API ~Window();

	ENGINEGRAPHICS_API inline Renderer* getRenderer() { return &_interface; }
};
