#pragma once

#include <ReflectionSpec.hpp>
#include "WidgetTransform.hpp"

class Renderer;
class HUD;
class Material;
class ShaderProgram;

class STIX_ENABLE_IMAGE_CAPTURE Widget
{
protected:
	HUD* hud;
	friend class HUD;

public:
	WidgetTransform transform;

	ENGINEGUI_API Widget();
	ENGINEGUI_API virtual ~Widget();

	ENGINEGUI_API const ShaderProgram* getShader() const;
	virtual const Material* getMaterial() const = 0;
	virtual void renderImmediate(Renderer* renderer) = 0;
};
