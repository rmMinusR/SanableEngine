#pragma once

#include <string>
#include "dllapi.h"

class Font;
class Texture;
class Sprite;

struct SDL_Color;
struct SDL_Rect;
struct SDL_Renderer;

class Renderer
{
private:
	SDL_Renderer* handle;

public:
	ENGINEGRAPHICS_API Renderer();
	ENGINEGRAPHICS_API Renderer(SDL_Renderer*);

	ENGINEGRAPHICS_API void beginFrame();
	ENGINEGRAPHICS_API void finishFrame();

	ENGINEGRAPHICS_API void drawRect(const SDL_Rect& rect, const SDL_Color& color);
	ENGINEGRAPHICS_API void drawText(const Font& font, const SDL_Color& color, const std::wstring& text, int x, int y, bool highQuality = false);
	ENGINEGRAPHICS_API void drawTexture(const Texture& tex, int x, int y);
	ENGINEGRAPHICS_API void drawSprite(const Sprite& sprite, int x, int y);
};
