#include "RectangleRenderer.hpp"

#include <SDL_rect.h>
#include <SDL_render.h>

#include "EngineCore.hpp"
#include "GameObject.hpp"

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

void RectangleRenderer::Render(Renderer* renderer)
{
	Vector3<float> pos = getGameObject()->getTransform()->getPosition();

	SDL_Rect r = {
		(int) pos.getX(),
		(int) pos.getY(),
		(int) w,
		(int) h
	};

	renderer->drawRect(r, color);
}
