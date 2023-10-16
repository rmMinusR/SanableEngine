#include "RectangleRenderer.hpp"

#include <SDL_rect.h>
#include <SDL_render.h>

#include "game/GameObject.hpp"
#include "Renderer.hpp"

RectangleRenderer::RectangleRenderer(float w, float h, SDL_Color color) :
	Component(),
	w(w),
	h(h),
	color(color)
{
}

RectangleRenderer::~RectangleRenderer()
{
}

void RectangleRenderer::SetColor(SDL_Color newColor)
{
	color = newColor;
}

Material* RectangleRenderer::getMaterial() const
{
	return nullptr;
}

void RectangleRenderer::renderImmediate(Renderer* renderer) const
{
	Vector3<float> pos = getGameObject()->getTransform()->getPosition();

	renderer->drawRect(pos, w, h, color);
}
