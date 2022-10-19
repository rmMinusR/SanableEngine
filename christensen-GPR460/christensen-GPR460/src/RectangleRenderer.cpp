#include "RectangleRenderer.hpp"

RectangleRenderer::RectangleRenderer(GameObject& owner, float w, float h, SDL_Color color) :
	Component(owner),
	w(w),
	h(h),
	color(color)
{
}

RectangleRenderer::~RectangleRenderer()
{
}
