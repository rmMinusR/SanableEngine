#pragma once

#include "Component.hpp"
#include <SDL_pixels.h>

class RectangleRenderer : public Component
{
private:
	float w, h;
	SDL_Color color;

public:
	RectangleRenderer(GameObject& owner, float w, float h, SDL_Color color);
	~RectangleRenderer();
};
