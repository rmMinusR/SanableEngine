#include "Renderer.hpp"

Renderer::Renderer() :
	Renderer(nullptr)
{
}

Renderer::Renderer(SDL_Renderer* handle) :
	handle(handle)
{
}

void Renderer::beginFrame()
{
	SDL_SetRenderDrawColor(handle, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(handle);
}

void Renderer::finishFrame()
{
	SDL_RenderPresent(handle);
}

void Renderer::drawRect(const SDL_Rect& rect, const SDL_Color& color)
{
	SDL_SetRenderDrawColor(handle, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(handle, &rect);
}
