#pragma once

#include "dllapi.h"

#include <SDL_pixels.h>

#include "Component.hpp"

class RectangleRenderer :
	public Component,
	public IRenderable
{
private:
	float w, h;
	SDL_Color color;

public:
	PRIMITIVES_API RectangleRenderer(float w, float h, SDL_Color color);
	PRIMITIVES_API ~RectangleRenderer();

	PRIMITIVES_API void Render(Renderer*) override;

	PRIMITIVES_API inline void SetColor(SDL_Color newColor) { color = newColor; }
};
