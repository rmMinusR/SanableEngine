#include "Texture.hpp"

#include <cassert>

#include <SDL_render.h>
#include <SDL_image.h>

#include "stb_image.h"

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

int Texture::getNChannels() const
{
	return nChannels;
}

CTexture::CTexture()
{
	width = 0;
	height = 0;
	nChannels = 0;
	data = nullptr;
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

GTexture::GTexture(Renderer* ctx, int width, int height, int nChannels, void* data)
{
	this->width = width;
	this->height = height;
	this->nChannels = nChannels;
	this->id = 0;

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

GTexture::GTexture(Renderer* ctx, const CTexture& src)
{
	this->width = src.width;
	this->height = src.height;
	this->nChannels = src.nChannels;
	this->id = 0;

	//Prep for GPU
	glGenTextures(1, &id);
	assert(id);

	//Send data to GPU
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, src.width, src.height, 0, GL_RGB, GL_UNSIGNED_BYTE, src.data);
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
