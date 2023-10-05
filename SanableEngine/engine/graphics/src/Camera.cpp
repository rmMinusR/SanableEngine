#include "Camera.hpp"

#include <cassert>

#undef WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl/GL.h>
#include <glm/gtc/type_ptr.hpp>

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
	glm::quat rot = gameObject->getTransform()->getRotation();

	//Main matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (mode == Mode::GUI)
	{
		glOrtho(0, w, h, 0, 0, zFar);
	}
	else if (mode == Mode::Ortho)
	{
		float scl = size / diag;
		w *= scl / 2;
		h *= scl / 2;
		glOrtho(-w, w, h, -h, 0, zFar);
	}
	else if (mode == Mode::Persp)
	{
		float v = tanf(size/2) * zNear;
		glFrustum(-v*aspectRatio, v*aspectRatio, v, -v, zNear, zFar);
	}
	else assert(false);

	//Transform unless GUI
	if (mode != Mode::GUI)
	{
		glTranslatef(-pos.getX(), -pos.getY(), -pos.getZ());
		glMultMatrixf(glm::value_ptr(glm::mat4_cast(rot)));
	}
}

void Camera::beginFrame()
{
	setProjectionMatrix();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}
