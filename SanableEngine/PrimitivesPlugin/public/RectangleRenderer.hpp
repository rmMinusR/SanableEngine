#pragma once

#include "dllapi.h"
#include "Color.inl"
#include "game/Component.hpp"

class RectangleRenderer :
	public Component,
	public I3DRenderable
{
	PRIMITIVES_API RectangleRenderer() = default;
private:
	float w, h;
	Color4<uint8_t> color;

public:
	PRIMITIVES_API RectangleRenderer(float w, float h, Color4<uint8_t> color);
	PRIMITIVES_API ~RectangleRenderer();

	PRIMITIVES_API void SetColor(Color4<uint8_t> newColor);

	PRIMITIVES_API virtual const Material* getMaterial() const override;
protected:
	PRIMITIVES_API virtual void loadModelTransform(Renderer*) const override;
	PRIMITIVES_API virtual void renderImmediate(Renderer*) const override;
};
