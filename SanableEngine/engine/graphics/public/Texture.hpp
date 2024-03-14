#pragma once

#include <filesystem>
#include <cstdint>

#include <SDL_render.h>
#include <GL/glew.h>

#include "dllapi.h"


class Renderer;
class CTexture;
class GTexture;


class CTexture
{
	friend class GTexture;

	int width;
	int height;
	int nChannels;
	
	uint8_t* data;

public:
	ENGINEGRAPHICS_API CTexture();
	ENGINEGRAPHICS_API CTexture(const std::filesystem::path& path);
	ENGINEGRAPHICS_API ~CTexture();

	ENGINEGRAPHICS_API int getWidth() const;
	ENGINEGRAPHICS_API int getHeight() const;
	ENGINEGRAPHICS_API int getNChannels() const;

	ENGINEGRAPHICS_API operator bool() const;

	ENGINEGRAPHICS_API CTexture(CTexture&& mov);
	ENGINEGRAPHICS_API CTexture& operator=(CTexture&& mov);
	CTexture(const CTexture& cpy) = delete;
	CTexture& operator=(const CTexture& cpy) = delete;
};


class GTexture
{
	GLuint id;
	friend class Renderer;

	int width;
	int height;
	int nChannels;
public:
	ENGINEGRAPHICS_API GTexture(Renderer* ctx, int width, int height, int nChannels, void* data = nullptr);
	ENGINEGRAPHICS_API GTexture(Renderer* ctx, const CTexture& src);
	ENGINEGRAPHICS_API ~GTexture();

	ENGINEGRAPHICS_API operator bool() const;

	ENGINEGRAPHICS_API GTexture(GTexture&& mov);
	ENGINEGRAPHICS_API GTexture& operator=(GTexture&& mov);
	GTexture(const GTexture& cpy) = delete;
	GTexture& operator=(const GTexture& cpy) = delete;
};
