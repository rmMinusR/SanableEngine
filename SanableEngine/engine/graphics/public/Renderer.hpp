#pragma once

#include <string>
#include <SDL_video.h>
#include "dllapi.h"

class EngineCore;
class Window;
class Font;
class Texture;

struct SDL_Color;
struct SDL_Rect;

class Renderer
{
private:
	Window* owner;
	SDL_GLContext context;

public:
	ENGINEGRAPHICS_API Renderer();
	ENGINEGRAPHICS_API Renderer(Window* owner, const SDL_GLContext& context);

	inline Window* getOwner() const { return owner; }
	
	ENGINEGRAPHICS_API void drawRect(const SDL_Rect& rect, const SDL_Color& color);
	ENGINEGRAPHICS_API void drawText(const Font& font, const SDL_Color& color, const std::wstring& text, int x, int y, bool highQuality = false);
	ENGINEGRAPHICS_API void drawTexture(const Texture& tex, int x, int y);

private:
	friend class EngineCore;
	void beginFrame();
	void finishFrame();
};
