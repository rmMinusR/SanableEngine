#pragma once

#include <filesystem>

#include <SDL_render.h>
#include <GL/glew.h>

#include "dllapi.h"
#include "Vector2.inl"

class Renderer;

class Texture
{
	friend class Renderer;

	GLuint id;

	int width;
	int height;
	int nChannels;

public:
	ENGINEGRAPHICS_API static Texture* fromFile(const std::filesystem::path&, Renderer* ctx);
	ENGINEGRAPHICS_API Texture(Renderer* ctx, int width, int height, int nChannels, void* data);
	ENGINEGRAPHICS_API ~Texture();

	ENGINEGRAPHICS_API Texture(Texture&& mov);
	ENGINEGRAPHICS_API Texture& operator=(Texture&& mov);
	Texture(const Texture& cpy) = delete;
	Texture& operator=(const Texture& cpy) = delete;

	ENGINEGRAPHICS_API int getWidth() const;
	ENGINEGRAPHICS_API int getHeight() const;
	ENGINEGRAPHICS_API Vector2<int> getSize() const;
	ENGINEGRAPHICS_API int getNChannels() const;

	ENGINEGRAPHICS_API operator bool() const;
};
