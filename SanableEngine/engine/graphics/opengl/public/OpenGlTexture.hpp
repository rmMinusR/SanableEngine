#pragma once

#include "Texture.hpp"
#include "dllapi.h"

class OpenGlTexture : public GTexture
{
	friend class OpenGlRenderer;
	GLuint id;

public:
	ENGINEOPENGL_API OpenGlTexture();
	ENGINEOPENGL_API OpenGlTexture(OpenGlRenderer* ctx, int width, int height, int nChannels, const void* data);
	ENGINEOPENGL_API OpenGlTexture(OpenGlRenderer* ctx, const CTexture& tex);
	ENGINEOPENGL_API virtual ~OpenGlTexture();

	ENGINEOPENGL_API OpenGlTexture(OpenGlTexture&& mov);
	ENGINEOPENGL_API virtual GTexture& operator=(GTexture&& mov) override;
	ENGINEOPENGL_API OpenGlTexture& operator=(OpenGlTexture&& mov);
	
	ENGINEOPENGL_API virtual operator bool() const override;
};
