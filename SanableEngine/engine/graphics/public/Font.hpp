#pragma once

#include <filesystem>

#include <SDL_ttf.h>

#include "dllapi.h"


class Font
{
	friend class Renderer;
	TTF_Font* handle;
	int size;
public:
	ENGINEGRAPHICS_API Font(const std::filesystem::path&, int size);
	ENGINEGRAPHICS_API ~Font();
};
