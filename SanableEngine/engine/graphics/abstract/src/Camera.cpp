#include "Camera.hpp"

#include <cassert>

#include <glm/gtc/type_ptr.hpp>

Camera::Camera()
{
	//glGenFramebuffers(1, &fb.handle);
	//glBindFramebuffer(GL_FRAMEBUFFER, fb.handle);
	//
	//glGenTextures(1, &fb.texColor);
	//glBindTexture(GL_TEXTURE_2D, fb.texColor);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	//
	//assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

Camera::~Camera()
{
	//if (fb.handle) glDeleteFramebuffers(1, &fb.handle);
}

void Camera::setGUIProj()
{
	mode = Mode::GUI;
}

void Camera::setOrtho(float cornerDist)
{
	mode = Mode::Ortho;
	size = cornerDist;
}

void Camera::setPersp(float fovDeg)
{
	mode = Mode::Persp;
	size = fovDeg;
}

glm::mat4 Camera::getMatrix(Rect<float> viewport) const
{
	float w = viewport.size.x;
	float h = viewport.size.y;
	float aspectRatio = w / h;
	float diag = sqrtf(w * w + h * h);

	if (mode == Mode::GUI)
	{
		return glm::ortho<float>(
			viewport.topLeft.x, viewport.topLeft.x + w,
			viewport.topLeft.y + h, viewport.topLeft.y, //+Y is down
			0, zFar
		);
		
	}
	else if (mode == Mode::Ortho)
	{
		float scl = size / diag;
		w *= scl / 2;
		h *= scl / 2;
		return glm::ortho<float>(
			-w, w,
			-h, h, //+Y is up
			0, zFar
		);
	}
	else if (mode == Mode::Persp)
	{
		float v = tanf(size / 2) * zNear;
		return glm::frustum<float>(
			-v * aspectRatio, v * aspectRatio,
			-v, v, //+Y is up
			zNear, zFar
		);
	}
	else
	{
		assert(false);
		return glm::identity<glm::mat4>();
	}
}

Camera::Mode Camera::getMode() const
{
	return mode;
}

Camera::Camera(Camera&& mov)
{
	*this = std::move(mov);
}

Camera& Camera::operator=(Camera&& mov)
{
	fb = mov.fb;
	mov.fb = FramebufferData();

	size = mov.size;
	mode = mov.mode;

	return *this;
}
