#include "RectangleRenderer.hpp"

#include <SDL_rect.h>
#include <SDL_render.h>

#include "game/GameObject.hpp"
#include "Renderer.hpp"

RectangleRenderer::RectangleRenderer(float w, float h, Color4<uint8_t> color) :
	Component(),
	w(w),
	h(h),
	color(color)
{
}

RectangleRenderer::~RectangleRenderer()
{
}

void RectangleRenderer::SetColor(Color4<uint8_t> newColor)
{
	color = newColor;
}

const Material* RectangleRenderer::getMaterial() const
{
	return nullptr;
}

void RectangleRenderer::loadModelTransform(Renderer* renderer) const
{
	renderer->setModelTransform(*gameObject->getTransform());
}

void RectangleRenderer::renderImmediate(Renderer* renderer) const
{
	Vector3<float> pos = getGameObject()->getTransform()->getPosition();

	renderer->drawRect(pos, w, h, color);
}
