#include "Camera.hpp"

#include <cassert>

#undef WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl/GL.h>

#include "Renderer.hpp"
#include "Window.hpp"

Camera* Camera::main = nullptr;

Camera* Camera::getMain()
{
	return main;
}

void Camera::setMain()
{
	main = this;
}

Camera::Camera()
{
	if (!main) main = this;
}

Camera::~Camera()
{
	if (main == this) main = nullptr;
}

void Camera::setGUIProj()
{
	mode = Mode::GUI;
	zNear = 0;
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

void Camera::setProjectionMatrix()
{
	Window* window = gameObject->getContext()->getMainWindow();
	float w = window->getWidth();
	float h = window->getHeight();
	float aspectRatio = w / h;
	float diag = sqrtf(w*w + h*h);

	Vector3<float> pos = gameObject->getTransform()->getPosition();

	switch (mode)
	{
	case Mode::GUI:
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, w, h, 0, zNear, zFar);
		break;
	}

	case Mode::Ortho:
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		float scl = size / diag;
		w *= scl / 2;
		h *= scl / 2;
		glOrtho(-w, w, -h, h, zNear, zFar);

		glTranslatef(pos.getX(), pos.getY(), pos.getZ());
		break;
	}

	case Mode::Persp:
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		float v = tanf(size/2) * zNear;
		glFrustum(-v*aspectRatio, v*aspectRatio, -v, v, zNear, zFar);
		
		glTranslatef(pos.getX(), pos.getY(), pos.getZ());
		break;
	}

	default:
		assert(false);
		break;
	}
}