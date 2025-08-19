#include "OpenGlFramebuffer.hpp"

#include "OpenGlTexture.hpp"
#include "OpenGlRenderer.hpp"

OpenGlFramebuffer::OpenGlFramebuffer() :
	renderTexture(nullptr),
	frameBuffer(0),
	depthBuffer(0)
{
}

OpenGlFramebuffer::OpenGlFramebuffer(OpenGlRenderer* renderer, Vector2<int> initialSize, const Settings& settings)
{
	// Setup framebuffer
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	// Setup color buffer
	renderTexture = renderer->newTexture(initialSize.x, initialSize.y, settings.numChannels, nullptr);

	// Setup depth buffer
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, initialSize.x, initialSize.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	// Bind color to framebuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, static_cast<OpenGlTexture*>(renderTexture)->id, 0);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	
	// Check OK
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	assert(status == GL_FRAMEBUFFER_COMPLETE);
}

OpenGlFramebuffer::~OpenGlFramebuffer()
{
	if (renderTexture)
	{
		delete renderTexture;
	}

	if (frameBuffer)
	{
		glDeleteFramebuffers(1, &frameBuffer);
	}
}

void OpenGlFramebuffer::resize(Vector2<int> size)
{
	assert(renderTexture);
	renderTexture->resize(size);
}

OpenGlFramebuffer::operator bool() const
{
	return renderTexture && *renderTexture && frameBuffer;
}

const GTexture* OpenGlFramebuffer::getTexture() const
{
	return renderTexture;
}

GTexture* OpenGlFramebuffer::getTexture()
{
	return renderTexture;
}
