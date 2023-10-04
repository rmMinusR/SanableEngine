#include "Renderer.hpp"

#undef WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <SDL_render.h>
#include <SDL_pixels.h>
#include <gl/GL.h>

#include "Texture.hpp"
#include "Window.hpp"

Renderer::Renderer() :
	context(nullptr)
{
}

Renderer::Renderer(Window* owner, const SDL_GLContext& context) :
	owner(owner),
	context(context)
{
}

void Renderer::beginFrame()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, owner->getWidth(), owner->getHeight(), 0, 0, 100);
}

void Renderer::finishFrame()
{
	SDL_GL_SwapWindow(owner->handle);
}

void Renderer::drawRect(const SDL_Rect& rect, const SDL_Color& color)
{
	glBegin(GL_QUADS);
	glColor4f(color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f);
	glVertex2i(rect.x       , rect.y       );
	glVertex2i(rect.x+rect.w, rect.y       );
	glVertex2i(rect.x+rect.w, rect.y+rect.h);
	glVertex2i(rect.x       , rect.y+rect.h);
	glEnd();
}

void Renderer::drawText(const Font& font, const SDL_Color& color, const std::wstring& text, int x, int y, bool highQuality)
{
	////Render on CPU
	//SDL_Surface* surf;
	//if (highQuality) surf = TTF_RenderUNICODE_Blended(font.handle, (uint16_t*)text.c_str(), color);
	//else             surf = TTF_RenderUNICODE_Solid  (font.handle, (uint16_t*)text.c_str(), color);
	//
	////Render
	//glPushMatrix();
	//glTranslatef(x, y, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, surf->pixels); //Draw using CPU.
	//
	//glPopMatrix();
	//
	////Cleanup. Note: Does OpenGL need us to keep this around?
	//SDL_FreeSurface(surf);
}

void Renderer::drawTexture(const Texture& tex, int x, int y)
{
	glBindTexture(GL_TEXTURE_2D, tex.id);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(x          , y           );
	glTexCoord2i(1, 0); glVertex2i(x+tex.width, y           );
	glTexCoord2i(1, 1); glVertex2i(x+tex.width, y+tex.height);
	glTexCoord2i(0, 1); glVertex2i(x          , y+tex.height);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
