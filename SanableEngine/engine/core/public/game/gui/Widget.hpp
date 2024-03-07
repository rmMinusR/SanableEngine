#pragma once

#include "WidgetTransform.hpp"

class Renderer;
class HUD;
class Material;
class ShaderProgram;

class Widget
{
protected:
	HUD* context;

public:
	WidgetTransform transform;

	ENGINEGUI_API Widget();
	ENGINEGUI_API virtual ~Widget();

	ENGINEGUI_API const ShaderProgram* getShader() const;
	virtual const Material* getMaterial() const = 0;
	virtual void renderImmediate(Renderer* renderer) = 0;
};
