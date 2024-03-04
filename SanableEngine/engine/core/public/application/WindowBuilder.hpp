#pragma once

#include <memory>
#include <optional>
#include <string>
#include "../dllapi.h"
#include "Vector3.inl"
#include "WindowRenderPipeline.hpp"
#include "WindowInputProcessor.hpp"
#include "GLSettings.hpp"

class Application;
class Window;

class WindowBuilder
{
	GLSettings glSettings; //Provided by engine. DO NOT MESS WITH outside of platform main.

	//Required
	Application* engine;
	std::string name;
	Vector3<int> size;
	std::unique_ptr<WindowRenderPipeline> renderPipeline;

	//Optional
	std::optional<Vector3<int>> position;
	std::unique_ptr<WindowInputProcessor> inputProcessor;
	
	WindowBuilder(Application* engine, const std::string& name, int width, int height, const GLSettings& glSettings, std::unique_ptr<WindowRenderPipeline>&& renderPipeline);
	friend class Application;
public:
	ENGINEGRAPHICS_API ~WindowBuilder();

	ENGINEGRAPHICS_API void setInputProcessor(std::unique_ptr<WindowInputProcessor>&& inputProcessor);

	ENGINEGRAPHICS_API Window* build(); //Caller is responsible for returned ptr.
	ENGINEGRAPHICS_API static void destroy(Window* window);
};
