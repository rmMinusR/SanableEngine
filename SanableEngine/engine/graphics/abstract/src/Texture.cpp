#include "Texture.hpp"

#include <cassert>

#include <SDL_render.h>
#include <SDL_image.h>

#include "stb_image.h"

#include "Renderer.hpp"

Texture::Texture(int width, int height, int nChannels) :
	width(width),
	height(height),
	nChannels(nChannels)
{
}

Texture::~Texture()
{
}

int Texture::getWidth() const
{
	return width;
}

int Texture::getHeight() const
{
	return height;
}

Vector2<int> Texture::getSize() const
{
	return Vector2<int>(width, height);
}

int Texture::getNChannels() const
{
	return nChannels;
}

size_t Texture::getDataUnitWidth() const
{
	return sizeof(uint8_t) * nChannels;
}

CTexture::CTexture(int width, int height, int nChannels, void* data) :
	Texture(width, height, nChannels),
	data(data)
{
	assert(data);
}

CTexture CTexture::fromFile(const std::filesystem::path& path)
{
	int width, height, nChannels;
	void* data = stbi_load(path.u8string().c_str(), &width, &height, &nChannels, 0);
	return CTexture(width, height, nChannels, data);
}

CTexture::CTexture(int width, int height, int nChannels) :
	CTexture(width, height, nChannels, malloc(width*height*nChannels))
{
}

CTexture::~CTexture()
{
	if (data) free(data); //Same as stbi_image_free
}

CTexture::CTexture(CTexture&& mov) :
	Texture(0, 0, 0)
{
	*this = std::move(mov);
}

CTexture& CTexture::operator=(CTexture&& mov)
{
	width     = mov.width;
	height    = mov.height;
	nChannels = mov.nChannels;

	if (data) free(data);
	data = mov.data;
	mov.data = nullptr;

	return *this;
}

CTexture::CTexture(const CTexture& cpy) :
	Texture(0, 0, 0)
{
	*this = cpy;
}

CTexture& CTexture::operator=(const CTexture& cpy)
{
	width     = cpy.width;
	height    = cpy.height;
	nChannels = cpy.nChannels;

	if (data) free(data);
	data = (uint8_t*)malloc(width*height*getDataUnitWidth());
	memcpy(data, cpy.data, width*height*getDataUnitWidth());
	
	return *this;
}

CTexture::operator bool() const
{
	return data;
}

void CTexture::resize(Vector2<int> size)
{
	size_t oldRowDataWidth = getWidth() * getDataUnitWidth();
	size_t newRowDataWidth = size.x * getDataUnitWidth();

	void* newData = malloc(width*height*getDataUnitWidth());

	if (newRowDataWidth < oldRowDataWidth)
	{
		for (int row = 0; row < getHeight(); ++row)
		{
			uint8_t* oldRowStart = static_cast<uint8_t*>(   data) + oldRowDataWidth*row;
			uint8_t* newRowStart = static_cast<uint8_t*>(newData) + newRowDataWidth*row;
			memcpy_s(newRowStart, newRowDataWidth, oldRowStart, oldRowDataWidth);
		}
	}
	else if (newRowDataWidth > oldRowDataWidth)
	{
		for (int row = getHeight() - 1; row << getHeight() >= 0; --row)
		{
			uint8_t* oldRowStart = static_cast<uint8_t*>(   data) + oldRowDataWidth*row;
			uint8_t* newRowStart = static_cast<uint8_t*>(newData) + newRowDataWidth*row;
			uint8_t* nextRowStart = static_cast<uint8_t*>(newData) + newRowDataWidth*(row+1);
			memcpy_s(newRowStart, newRowDataWidth, oldRowStart, oldRowDataWidth);

			size_t blankBytes = newRowDataWidth-oldRowDataWidth;
			memset(nextRowStart-blankBytes, 0, blankBytes);
		}
	}

	free(data);
	data = newData;
	width = size.x;
	height = size.y;
}

void* CTexture::pixel(int x, int y)
{
	return static_cast<uint8_t*>(data) + nChannels * (x+y*width);
}

const void* CTexture::pixel(int x, int y) const
{
	return static_cast<const uint8_t*>(data) + nChannels * (x+y*width);
}

GTexture::GTexture() :
	Texture(0, 0, 0)
{
}

GTexture::GTexture(int width, int height, int nChannels, const void* data) :
	Texture(width, height, nChannels)
{
}

GTexture::~GTexture()
{
}
