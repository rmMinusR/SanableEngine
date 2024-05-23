#pragma once

#include <filesystem>

#include <SDL_render.h>
#include <GL/glew.h>

#include "dllapi.h"
#include "math/Vector2.inl"

class Renderer;
class GTexture;


class Texture
{
protected:
	int width;
	int height;
	int nChannels;
	Texture(int width, int height, int nChannels);
public:
	ENGINEGRAPHICS_API virtual ~Texture();

	ENGINEGRAPHICS_API int getWidth() const;
	ENGINEGRAPHICS_API int getHeight() const;
	ENGINEGRAPHICS_API Vector2<int> getSize() const;
	ENGINEGRAPHICS_API int getNChannels() const;

	ENGINEGRAPHICS_API virtual operator bool() const = 0;
};


//CPU-sided texture
class CTexture : public Texture
{
	friend class GTexture;

	void* data;

	CTexture(int width, int height, int nChannels, void* data);
public:
	[[nodiscard]] ENGINEGRAPHICS_API static CTexture fromFile(const std::filesystem::path&);
	ENGINEGRAPHICS_API CTexture(int width, int height, int nChannels);
	ENGINEGRAPHICS_API ~CTexture();

	ENGINEGRAPHICS_API CTexture(CTexture&& mov);
	ENGINEGRAPHICS_API CTexture& operator=(CTexture&& mov);
	ENGINEGRAPHICS_API CTexture(const CTexture& cpy);
	ENGINEGRAPHICS_API CTexture& operator=(const CTexture& cpy);

	ENGINEGRAPHICS_API virtual operator bool() const override;

	ENGINEGRAPHICS_API void* pixel(int x, int y);
	ENGINEGRAPHICS_API const void* pixel(int x, int y) const;
};


//GPU-sided texture
class GTexture : public Texture
{
	friend class Renderer;

	GLuint id;

public:
	ENGINEGRAPHICS_API static GTexture* fromFile(const std::filesystem::path&, Renderer* ctx);
	ENGINEGRAPHICS_API GTexture();
	ENGINEGRAPHICS_API GTexture(Renderer* ctx, int width, int height, int nChannels, void* data);
	ENGINEGRAPHICS_API GTexture(Renderer* ctx, const CTexture& tex);
	ENGINEGRAPHICS_API ~GTexture();

	ENGINEGRAPHICS_API GTexture(GTexture&& mov);
	ENGINEGRAPHICS_API GTexture& operator=(GTexture&& mov);
	GTexture(const GTexture& cpy) = delete;
	GTexture& operator=(const GTexture& cpy) = delete;

	ENGINEGRAPHICS_API virtual operator bool() const override;
};
