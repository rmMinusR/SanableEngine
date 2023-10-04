#pragma once

#include <filesystem>

#include <SDL_render.h>

#include "dllapi.h"

struct SDL_Renderer;
struct SDL_Surface;

class Renderer;

class Texture
{
	friend class Renderer;
	SDL_Renderer* renderer;
	SDL_Surface* cpu;
	SDL_Texture* gpu;
public:
	ENGINEGRAPHICS_API Texture(const std::filesystem::path&, SDL_Renderer* renderer); //FIXME should use own renderer instead?
	ENGINEGRAPHICS_API ~Texture();

	ENGINEGRAPHICS_API Texture(const Texture& cpy);
	ENGINEGRAPHICS_API Texture(Texture&& mov);
	ENGINEGRAPHICS_API Texture& operator=(const Texture& cpy);
	ENGINEGRAPHICS_API Texture& operator=(Texture&& mov);
};
