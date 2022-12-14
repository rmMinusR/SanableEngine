#pragma once

#include <SDL_pixels.h>

#include "Component.hpp"
#include "PluginCore.hpp"

class RectangleRenderer :
	public Component,
	public IRenderable
{
private:
	float w, h;
	SDL_Color color;

public:
	PLUGIN_API_CTOR RectangleRenderer(float w, float h, SDL_Color color);
	~RectangleRenderer();

	void Render() override;

	inline void SetColor(SDL_Color newColor) { color = newColor; }
};
