#pragma once

#include <memory>
#include <optional>
#include <string>
#include "../dllapi.h"
#include "math/Vector3.inl"
#include "WindowRenderPipeline.hpp"
#include "WindowInputProcessor.hpp"
#include "GLSettings.hpp"

class Application;
class Window;

class WindowBuilder
{
	GLSettings glSettings; //Provided by engine. DO NOT MESS WITH outside of platform main.

	//Required
	Application* engine; //Not owned
	std::string name;
	Vector3<int> size;
	WindowRenderPipeline* renderPipeline; //Owned by self, nulled on build

	//Optional
	std::optional<Vector3<int>> position;
	WindowInputProcessor* inputProcessor; //Owned by self, nulled on build
	
	WindowBuilder(Application* engine, const std::string& name, int width, int height, const GLSettings& glSettings);
	friend class Application;
public:
	ENGINEGRAPHICS_API ~WindowBuilder();

	ENGINEGRAPHICS_API void setInputProcessor(WindowInputProcessor* inputProcessor);
	ENGINEGRAPHICS_API void setRenderPipeline(WindowRenderPipeline* renderPipeline);
	
	[[nodiscard]] ENGINEGRAPHICS_API Window* build();
};
