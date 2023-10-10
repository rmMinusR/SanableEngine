#include "Camera.hpp"

#include <cassert>

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
	if (main == this) main = nullptr;

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
		glOrtho(0, w, h, 0, 0, zFar); //+Y is down
	}
	else if (mode == Mode::Ortho)
	{
		float scl = size / diag;
		w *= scl / 2;
		h *= scl / 2;
		glOrtho(-w, w, -h, h, 0, zFar); //+Y is up
	}
	else if (mode == Mode::Persp)
	{
		float v = tanf(size/2) * zNear;
		glFrustum(-v*aspectRatio, v*aspectRatio, -v, v, zNear, zFar); //+Y is up
	}
	else assert(false);

	//Transform unless GUI
	if (mode != Mode::GUI)
	{
		glTranslatef(-pos.x, -pos.y, -pos.z);
		glMultMatrixf(glm::value_ptr(glm::mat4_cast(rot)));
	}
}

void Camera::beginFrame()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	setProjectionMatrix();
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
}
