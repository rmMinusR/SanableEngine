#pragma once

#include <memory>
#include <optional>
#include <string>
#include "../dllapi.h"
#include "math/Vector2.inl"

class Application;
class Window;
class WindowRenderPipeline;
class WindowInputProcessor;

struct WindowSettings
{
public:
	//Required
	std::string name;
	Vector2<int> size;
	WindowRenderPipeline* renderPipeline; //Owned by self, nulled on build

	//Optional
	std::optional<Vector2<int>> position;
	WindowInputProcessor* inputProcessor; //Owned by self, nulled on build
	
	ENGINECORE_API WindowSettings(const std::string& name, int width, int height);
	ENGINECORE_API ~WindowSettings();
};
