#pragma once

#include "dllapi.h"

#include <SDL_pixels.h>

#include "game/Component.hpp"

class RectangleRenderer :
	public Component,
	public I3DRenderable
{
	PRIMITIVES_API RectangleRenderer() = default;
private:
	float w, h;
	SDL_Color color;

public:
	PRIMITIVES_API RectangleRenderer(float w, float h, SDL_Color color);
	PRIMITIVES_API ~RectangleRenderer();

	PRIMITIVES_API void SetColor(SDL_Color newColor);

	PRIMITIVES_API virtual const Material* getMaterial() const override;
protected:
	PRIMITIVES_API virtual void renderImmediate(Renderer*) const override;
};
