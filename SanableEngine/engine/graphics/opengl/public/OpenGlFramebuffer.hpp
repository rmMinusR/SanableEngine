#pragma once

#include <GL/glew.h>
#include "math/Vector2.inl"
#include "Framebuffer.hpp"
#include "dllapi.h"

class OpenGlRenderer;
class GTexture;

class OpenGlFramebuffer : public Framebuffer
{
	GTexture* renderTexture;
	GLuint frameBuffer;
	GLuint depthBuffer;

	friend class OpenGlRenderer;

public:
	ENGINEOPENGL_API OpenGlFramebuffer();
	ENGINEOPENGL_API OpenGlFramebuffer(OpenGlRenderer* renderer, Vector2<int> initialSize, const Settings& settings);
	ENGINEOPENGL_API virtual ~OpenGlFramebuffer();

	ENGINEOPENGL_API virtual void resize(Vector2<int> size) override;
	ENGINEOPENGL_API virtual operator bool() const override;

	ENGINEOPENGL_API virtual const GTexture* getTexture() const override;
	ENGINEOPENGL_API virtual GTexture* getTexture() override;
};
