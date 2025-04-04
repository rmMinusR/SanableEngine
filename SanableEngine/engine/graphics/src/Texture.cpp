#include "Texture.hpp"

#include <cassert>

#include <SDL_render.h>
#include <SDL_image.h>

#include "stb_image.h"

#include "Renderer.hpp"
#include "application/Window.hpp"

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
	data = (uint8_t*)malloc(width*height*nChannels);
	memcpy(data, cpy.data, width*height*nChannels);
	
	return *this;
}

CTexture::operator bool() const
{
	return data;
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

OpenGlTexture::OpenGlTexture(OpenGlRenderer* ctx, int width, int height, int nChannels, const void* data) :
	GTexture(width, height, nChannels, data),
	id(0)
{
	ctx->activate();
	Window::setActiveDrawTarget(ctx->getOwner());

	glGenTextures(1, &id);
	assert(id);

	glBindTexture(GL_TEXTURE_2D, id);

	//Set filtering/wrapping modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Send data to GPU
	//TODO better channels description detection
	int glChannelsDesc;
	switch (nChannels)
	{
		case 1: glChannelsDesc = GL_RED; break;
		case 2: glChannelsDesc = GL_RG; break;
		case 3: glChannelsDesc = GL_RGB; break;
		case 4: glChannelsDesc = GL_RGBA; break;
		default: assert(false); break;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, glChannelsDesc, width, height, 0, glChannelsDesc, GL_UNSIGNED_BYTE, data);

	ctx->errorCheck();
}

OpenGlTexture::OpenGlTexture(OpenGlRenderer* ctx, const CTexture& tex) :
	OpenGlTexture(ctx, tex.getWidth(), tex.getHeight(), tex.getNChannels(), tex.pixel(0, 0))
{
}

OpenGlTexture::~OpenGlTexture()
{
	glDeleteTextures(1, &id);
}

OpenGlTexture::OpenGlTexture(OpenGlTexture&& mov)
{
	*this = std::move(mov);
}

GTexture& OpenGlTexture::operator=(GTexture&& mov)
{
	this->operator=(static_cast<GTexture&&>(mov));
	return *this;
}

OpenGlTexture& OpenGlTexture::operator=(OpenGlTexture&& mov)
{
	if (this->id) glDeleteTextures(1, &id);

	this->id = mov.id;
	mov.id = 0;

	this->width = mov.width;
	this->height = mov.height;
	this->nChannels = mov.nChannels;

	return *this;
}

OpenGlTexture::operator bool() const
{
	return id != 0;
}
