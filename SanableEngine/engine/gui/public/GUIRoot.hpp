#pragma once

#include <functional>

#include "EngineCoreReflectionHooks.hpp"
#include "dllapi.h"

#include "Rect.hpp"

class Renderer;
class GUIElement;

class GUIRoot
{
	SANABLE_REFLECTION_HOOKS

public:
	float zNear = 0.1f;
	float zFar = 1000;

	ENGINEGUI_API GUIRoot();
	ENGINEGUI_API virtual ~GUIRoot();

	//Called by Renderer
	virtual void draw(Renderer* renderer, Rect<float> workingArea) const = 0;

	virtual void visit(const std::function<GUIElement*>& visitor, bool recurse) = 0;

	ENGINEGUI_API GUIRoot(GUIRoot&& mov);
	ENGINEGUI_API GUIRoot& operator=(GUIRoot&& mov);
	GUIRoot(const GUIRoot& cpy) = delete;
	GUIRoot& operator=(const GUIRoot& cpy) = delete;
};
