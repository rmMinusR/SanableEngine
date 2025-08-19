#pragma once

#include <filesystem>

#include <SDL_render.h>

#include "dllapi.h"
#include "math/Vector2.inl"

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

	ENGINEGRAPHICS_API size_t getDataUnitWidth() const;

	ENGINEGRAPHICS_API virtual operator bool() const = 0;
	ENGINEGRAPHICS_API virtual void resize(Vector2<int> size) = 0;
};


//CPU-sided texture
class CTexture : public Texture
{
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
	ENGINEGRAPHICS_API virtual void resize(Vector2<int> size);

	ENGINEGRAPHICS_API void* pixel(int x, int y);
	ENGINEGRAPHICS_API const void* pixel(int x, int y) const;
};


//GPU-sided texture
class GTexture : public Texture
{
protected:
	ENGINEGRAPHICS_API GTexture();
	ENGINEGRAPHICS_API GTexture(int width, int height, int nChannels, const void* data);
public:
	ENGINEGRAPHICS_API virtual ~GTexture();

	GTexture(GTexture&& mov) = delete;
	ENGINEGRAPHICS_API virtual GTexture& operator=(GTexture&& mov) = 0;
	GTexture(const GTexture& cpy) = delete;
	GTexture& operator=(const GTexture& cpy) = delete;
};
