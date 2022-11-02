#include "RectangleRenderer.hpp"

#include "EngineCore.hpp"
#include "GameObject.hpp"

RectangleRenderer::RectangleRenderer(GameObject* owner, float w, float h, SDL_Color color) :
	Component(owner),
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
	Vector3<float> pos = gameObject->getTransform()->getPosition();

	SDL_Rect r = {
		(int) pos.getX(),
		(int) pos.getY(),
		(int) w,
		(int) h
	};

	SDL_SetRenderDrawColor(engine.renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(engine.renderer, &r);
}
