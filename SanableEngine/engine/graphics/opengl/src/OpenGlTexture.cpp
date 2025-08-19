#include "OpenGlTexture.hpp"

#include <cassert>

#include <SDL_render.h>
#include <SDL_image.h>

#include "stb_image.h"

#include "OpenGlRenderer.hpp"

OpenGlTexture::OpenGlTexture() :
	GTexture(),
	id(0)
{
}

GLuint OpenGlTexture::createTextureHandle(OpenGlRenderer* ctx, int width, int height, int nChannels, const void* data)
{
	ctx->activate();

	GLuint id;
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

OpenGlTexture::OpenGlTexture(OpenGlRenderer* ctx, int width, int height, int nChannels, const void* data) :
	GTexture(width, height, nChannels, data)
{
	renderer = ctx;
	id = createTextureHandle(ctx, width, height, nChannels, data);
}

OpenGlTexture::OpenGlTexture(OpenGlRenderer* ctx, const CTexture& tex) :
	OpenGlTexture(ctx, tex.getWidth(), tex.getHeight(), tex.getNChannels(), tex.pixel(0, 0))
{
}

OpenGlTexture::~OpenGlTexture()
{
	glDeleteTextures(1, &id);
}

OpenGlTexture::OpenGlTexture(OpenGlTexture&& mov) :
	OpenGlTexture()
{
	*this = std::move(mov);
}

GTexture& OpenGlTexture::operator=(GTexture&& mov)
{
	*this = static_cast<GTexture&&>(mov);
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

void OpenGlTexture::resize(Vector2<int> size)
{
	assert(renderer);

	GLuint newTex = createTextureHandle(renderer, size.x, size.y, nChannels, nullptr);
	glCopyImageSubData(
		id, GL_TEXTURE_2D, 0, 0, 0, 0,
		newTex, GL_TEXTURE_2D, 0, 0, 0, 0,
		std::min(width, size.x), std::min(height, size.y), 1
	);
	renderer->errorCheck();

	glDeleteTextures(1, &id);
	id = newTex;
	width = size.x;
	height = size.y;
}
