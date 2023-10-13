#pragma once

#include "dllapi.h"

class Window;
class EngineCore;

class WindowRenderPipeline
{
public:
	ENGINEGRAPHICS_API WindowRenderPipeline();
	ENGINEGRAPHICS_API virtual ~WindowRenderPipeline();

protected:
	Window* window;
	ENGINEGRAPHICS_API virtual void setup(Window* window);
	virtual void render() = 0;
	friend class Window;
};
