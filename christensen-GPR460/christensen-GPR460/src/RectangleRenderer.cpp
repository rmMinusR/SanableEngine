#include "RectangleRenderer.hpp"

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

void RectangleRenderer::Render()
{
	Vector3<float> pos = getGameObject()->getTransform()->getPosition();

	SDL_Rect r = {
		(int) pos.getX(),
		(int) pos.getY(),
		(int) w,
		(int) h
	};

	SDL_SetRenderDrawColor(EngineCore::getInstance()->renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(EngineCore::getInstance()->renderer, &r);
}
