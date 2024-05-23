#pragma once

#include <filesystem>

#include <SDL_render.h>
#include <GL/glew.h>

#include "dllapi.h"
#include "math/Vector2.inl"

class Renderer;

class GTexture
{
	friend class Renderer;

	GLuint id;

	int width;
	int height;
	int nChannels;

public:
	ENGINEGRAPHICS_API static GTexture* fromFile(const std::filesystem::path&, Renderer* ctx);
	ENGINEGRAPHICS_API GTexture(Renderer* ctx, int width, int height, int nChannels, void* data);
	ENGINEGRAPHICS_API ~GTexture();

	ENGINEGRAPHICS_API GTexture(GTexture&& mov);
	ENGINEGRAPHICS_API GTexture& operator=(GTexture&& mov);
	GTexture(const GTexture& cpy) = delete;
	GTexture& operator=(const GTexture& cpy) = delete;

	ENGINEGRAPHICS_API int getWidth() const;
	ENGINEGRAPHICS_API int getHeight() const;
	ENGINEGRAPHICS_API Vector2<int> getSize() const;
	ENGINEGRAPHICS_API int getNChannels() const;

	ENGINEGRAPHICS_API operator bool() const;
};
