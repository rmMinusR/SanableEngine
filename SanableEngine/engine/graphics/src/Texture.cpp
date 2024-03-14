#include "Texture.hpp"

#include <cassert>

#include <SDL_render.h>
#include <SDL_image.h>

#include "stb_image.h"

CTexture::CTexture() :
	width(0),
	height(0),
	nChannels(0),
	data(nullptr)
{
}

CTexture::CTexture(const std::filesystem::path& path)
{
	data = stbi_load(path.u8string().c_str(), &width, &height, &nChannels, 0);
}

CTexture::~CTexture()
{
	if (data)
	{
		stbi_image_free(data);
		data = nullptr;
	}
}

int CTexture::getWidth() const
{
	return width;
}

int CTexture::getHeight() const
{
	return height;
}

int CTexture::getNChannels() const
{
	return nChannels;
}

CTexture::operator bool() const
{
	return data != nullptr;
}

CTexture::CTexture(CTexture&& mov)
{
	*this = std::move(mov);
}

CTexture& CTexture::operator=(CTexture&& mov)
{
	if (data) stbi_image_free(data);

	width     = mov.width;
	height    = mov.height;
	nChannels = mov.nChannels;
	data      = mov.data;

	mov.width = 0;
	mov.height = 0;
	mov.nChannels = 0;
	mov.data = nullptr;

	return *this;
}

GTexture::GTexture(Renderer* ctx, int width, int height, int nChannels, void* data) :
	id(0),
	width(width),
	height(height),
	nChannels(nChannels)
{
	//Prep for GPU
	glGenTextures(1, &id);
	assert(id);

	//Set filtering/wrapping modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Send data to GPU
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}

GTexture::GTexture(Renderer* ctx, const CTexture& src) :
	id(0),
	width(src.getWidth()),
	height(src.getHeight()),
	nChannels(src.getNChannels())
{
	//Prep for GPU
	glGenTextures(1, &id);
	assert(id);

	//Send data to GPU
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, src.data);
}

GTexture::~GTexture()
{
	glDeleteTextures(1, &id);
}

GTexture::operator bool() const
{
	return id != 0;
}

GTexture::GTexture(GTexture&& mov)
{
	*this = std::move(mov); //Defer
}

GTexture& GTexture::operator=(GTexture&& mov)
{
	if (this->id) glDeleteTextures(1, &id);

	this->id = mov.id;
	mov.id = 0;
	
	this->width = mov.width;
	this->height = mov.height;
	this->nChannels = mov.nChannels;

	return *this;
}
