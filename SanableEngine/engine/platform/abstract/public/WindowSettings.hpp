#pragma once

#include <memory>
#include <optional>
#include <string>

#include "math/Vector2.inl"

class Application;
class WindowRenderPipeline;
class WindowInputProcessor;

struct WindowSettings
{
	//Required
	std::string name;
	Vector2<int> size;
	WindowRenderPipeline* renderPipeline = nullptr; //Owned by self, nulled on build, NOT nulled on destroy!

	//Optional
	std::optional<Vector2<int>> position;
	WindowInputProcessor* inputProcessor = nullptr; //Owned by self, nulled on build, NOT nulled on destroy!
	
	WindowSettings(const std::string& name, int width, int height);
	~WindowSettings();
};
