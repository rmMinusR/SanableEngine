#pragma once

#include <memory>
#include <optional>
#include <string>
#include <EngineCoreReflectionHooks.hpp>
#include "dllapi.h"
#include "Vector3.inl"
#include "WindowRenderPipeline.hpp"

class EngineCore;
class Window;

class WindowBuilder
{
	SANABLE_REFLECTION_HOOKS

	//Required
	EngineCore* engine;
	std::string name;
	Vector3<int> size;
	std::unique_ptr<WindowRenderPipeline> renderPipeline;

	//Optional
	std::optional<Vector3<int>> position;
	
	WindowBuilder(EngineCore* engine, const std::string& name, int width, int height, std::unique_ptr<WindowRenderPipeline>&& renderPipeline);
	friend class EngineCore;
public:
	ENGINEGRAPHICS_API ~WindowBuilder();

	ENGINEGRAPHICS_API Window* build(); //Caller is responsible for returned ptr.
	ENGINEGRAPHICS_API static void destroy(Window* window);
};
