#include "Renderer.hpp"

#include <SDL_image.h>
#include <SDL_render.h>
#include <SDL_pixels.h>

#include <Font.hpp>
#include <Texture.hpp>
#include <Sprite.hpp>

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

void Renderer::drawText(const Font& font, const SDL_Color& color, const std::wstring& text, int x, int y, bool highQuality)
{
	SDL_Surface* surf;
	if (highQuality) surf = TTF_RenderUNICODE_Blended(font.handle, (uint16_t*)text.c_str(), color);
	else             surf = TTF_RenderUNICODE_Solid  (font.handle, (uint16_t*)text.c_str(), color);

	SDL_Texture* tex = SDL_CreateTextureFromSurface(handle, surf);

	SDL_Rect dst = { x, y, surf->w, surf->h };
	SDL_RenderCopy(handle, tex, nullptr, &dst);

	SDL_DestroyTexture(tex);
	SDL_FreeSurface(surf);
}

void Renderer::drawTexture(const Texture& tex, int x, int y)
{
	SDL_Rect dst = { x, y, tex.handle->w, tex.handle->h };
	SDL_RenderCopy(handle, tex.view, nullptr, &dst);
}

void Renderer::drawSprite(const Sprite& sprite, int x, int y)
{
	SDL_Rect dst = { x, y, sprite.bounds.w, sprite.bounds.h };
	SDL_RenderCopy(handle, sprite.spritesheet->view, &sprite.bounds, &dst);
}
