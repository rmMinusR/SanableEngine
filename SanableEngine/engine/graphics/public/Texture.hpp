#pragma once

#include <filesystem>
#include <cstdint>

#include <SDL_render.h>
#include <GL/glew.h>

#include "dllapi.h"


class Renderer;
class CTexture;
class GTexture;


class Texture
{
protected:
	int width;
	int height;
	int nChannels;

public:
	ENGINEGRAPHICS_API virtual ~Texture();

	ENGINEGRAPHICS_API int getWidth() const;
	ENGINEGRAPHICS_API int getHeight() const;
	ENGINEGRAPHICS_API int getNChannels() const;

	ENGINEGRAPHICS_API virtual operator bool() const = 0;
};


//CPU-sided texture
class CTexture : public Texture
{
	friend class GTexture;

	uint8_t* data;

public:
	ENGINEGRAPHICS_API CTexture();
	ENGINEGRAPHICS_API CTexture(const std::filesystem::path& path);
	ENGINEGRAPHICS_API virtual ~CTexture();

	ENGINEGRAPHICS_API virtual operator bool() const override;

	ENGINEGRAPHICS_API CTexture(CTexture&& mov);
	ENGINEGRAPHICS_API CTexture& operator=(CTexture&& mov);
	CTexture(const CTexture& cpy) = delete;
	CTexture& operator=(const CTexture& cpy) = delete;
};


//GPU-sided texture
class GTexture : public Texture
{
	GLuint id;
	friend class Renderer;

public:
	ENGINEGRAPHICS_API GTexture(Renderer* ctx, int width, int height, int nChannels, void* data = nullptr);
	ENGINEGRAPHICS_API GTexture(Renderer* ctx, const CTexture& src);
	ENGINEGRAPHICS_API virtual ~GTexture();

	ENGINEGRAPHICS_API virtual operator bool() const override;

	ENGINEGRAPHICS_API GTexture(GTexture&& mov);
	ENGINEGRAPHICS_API GTexture& operator=(GTexture&& mov);
	GTexture(const GTexture& cpy) = delete;
	GTexture& operator=(const GTexture& cpy) = delete;
};
