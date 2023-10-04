#pragma once

#include <filesystem>

#include <Windows.h>
#include <SDL_render.h>
#include <gl/GL.h>

#include "dllapi.h"

struct SDL_Renderer;
struct SDL_Surface;

class Renderer;

class Texture
{
	friend class Renderer;

	GLuint id;

	int width;
	int height;
	int nChannels;

public:
	ENGINEGRAPHICS_API Texture(const std::filesystem::path&, SDL_GLContext ctx); //FIXME should use own renderer instead?
	ENGINEGRAPHICS_API ~Texture();

	ENGINEGRAPHICS_API Texture(const Texture& cpy);
	ENGINEGRAPHICS_API Texture(Texture&& mov);
	ENGINEGRAPHICS_API Texture& operator=(const Texture& cpy);
	ENGINEGRAPHICS_API Texture& operator=(Texture&& mov);
};
