#pragma once

#include "application/WindowRenderPipeline.hpp"
#include "HUD.hpp"

class Application;

/// <summary>
/// For rendering only a GUI in a window
/// </summary>
class WindowGUIRenderPipeline : public WindowRenderPipeline
{
public:
	ENGINECORE_API WindowGUIRenderPipeline(Application* application);
	ENGINECORE_API virtual ~WindowGUIRenderPipeline();

	HUD hud;
	float zNear = 0;
	float zFar = 1000;

	ENGINECORE_API virtual void render(Rect<float> viewport) override;
};
