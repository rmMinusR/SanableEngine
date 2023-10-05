#pragma once

#include <variant>

#undef WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl/GL.h>

#include "Renderer.hpp"
#include "Component.hpp"

class Camera : public Component
{
public:
	enum Mode
	{
		GUI,
		Ortho,
		Persp
	};

private:
	float size; //Distance in ortho mode, or FOV angle in persp
	Mode mode;

	struct FramebufferData
	{
		GLuint handle = 0;
		GLuint texColor = 0;
		GLuint texDepth = 0;
	} fb;

	static Camera* main;


public:
	float zNear = 0.1f;
	float zFar = 1000;

	ENGINEGRAPHICS_API Camera();
	ENGINEGRAPHICS_API ~Camera();

	ENGINEGRAPHICS_API static Camera* getMain();
	ENGINEGRAPHICS_API void setMain();

	//Mode setters
	ENGINEGRAPHICS_API void setGUIProj();
	ENGINEGRAPHICS_API void setOrtho(float cornerDist);
	ENGINEGRAPHICS_API void setPersp(float fovDeg);

	//Called by Renderer
	ENGINEGRAPHICS_API void setProjectionMatrix();
	ENGINEGRAPHICS_API void beginFrame();

	ENGINEGRAPHICS_API Camera(Camera&& mov);
	ENGINEGRAPHICS_API Camera& operator=(Camera&& mov);
	Camera(const Camera& cpy) = delete;
	Camera& operator=(const Camera& cpy) = delete;
};
