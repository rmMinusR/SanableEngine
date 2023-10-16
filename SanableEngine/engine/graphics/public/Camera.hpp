#pragma once

#include <variant>

#include <GL/glew.h>
#include <EngineCoreReflectionHooks.hpp>

#include "Rect.hpp"
#include "Renderer.hpp"
#include "game/Component.hpp"

#undef main

class Camera : public Component
{
	SANABLE_REFLECTION_HOOKS

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
	ENGINEGRAPHICS_API void setProjectionMatrix(Vector3<float> viewportSize);
	ENGINEGRAPHICS_API void beginFrame(Vector3<float> viewportSize);

	ENGINEGRAPHICS_API Camera(Camera&& mov);
	ENGINEGRAPHICS_API Camera& operator=(Camera&& mov);
	Camera(const Camera& cpy) = delete;
	Camera& operator=(const Camera& cpy) = delete;
};
