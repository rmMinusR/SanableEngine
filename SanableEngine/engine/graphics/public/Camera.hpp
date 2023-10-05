#pragma once

#include <variant>

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
};
