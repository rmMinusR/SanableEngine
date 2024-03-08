#include "Renderer.hpp"

#include <SDL_render.h>
#include <SDL_pixels.h>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "application/Window.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "MeshRenderer.hpp"
#include "Font.hpp"

Renderer::Renderer() :
	owner(nullptr),
	context(nullptr)
{
}

Renderer::Renderer(Window* owner, SDL_GLContext context) :
	owner(owner),
	context(context)
{
}

void Renderer::drawRect(Vector3f center, float w, float h, const SDL_Color& color)
{
	ShaderProgram::clear();

	glBegin(GL_QUADS);
	glColor4f(color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f);
	glVertex3f(center.x-w/2, center.y-h/2, center.z);
	glVertex3f(center.x+w/2, center.y-h/2, center.z);
	glVertex3f(center.x+w/2, center.y+h/2, center.z);
	glVertex3f(center.x-w/2, center.y+h/2, center.z);
	glEnd();
}

void Renderer::drawText(const Font& font, const SDL_Color& color, const std::wstring& text, Vector3f pos)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int i = 0; i < text.length(); ++i)
	{
		const Font::CachedGlyph* glyph = font.getGlyph(text[i], this);
		
		drawTexture(
			glyph->texture,
			pos+Vector3f(glyph->bearingX, glyph->bearingY, 0),
			glyph->texture->getNativeWidth(),
			glyph->texture->getNativeHeight()
		);

		//Advance position
		pos.x += glyph->advance;
	}

	glDisable(GL_BLEND);
}

void Renderer::drawTexture(const Texture& tex, int x, int y)
{
	ShaderProgram::clear();
	drawTexture(&tex, Vector3f(x, y, 0), tex.width, tex.height);
}

void Renderer::drawTexture(const Texture* tex, Vector3f pos, float w, float h)
{
	if (tex) glBindTexture(GL_TEXTURE_2D, tex->id);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(pos.x  , pos.y  , pos.z);
	glTexCoord2i(1, 0); glVertex3f(pos.x+w, pos.y  , pos.z);
	glTexCoord2i(1, 1); glVertex3f(pos.x+w, pos.y+h, pos.z);
	glTexCoord2i(0, 1); glVertex3f(pos.x  , pos.y+h, pos.z);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

Texture* Renderer::loadTexture(const std::filesystem::path& path)
{
	return Texture::fromFile(path, this);
}

Texture* Renderer::newTexture(int width, int height, int nChannels, void* data)
{
	return new Texture(this, width, height, nChannels, data);
}

Texture* Renderer::renderFontGlyph(const Font& font)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Disable byte-alignment restriction: OpenGL textures have 4-byte align and size, but here we're grayscale
	return newTexture(
		font.font->glyph->bitmap.width,
		font.font->glyph->bitmap.rows,
		1,
		font.font->glyph->bitmap.buffer
	);
}

void Renderer::errorCheck()
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("GL error (code %u): %s\n", err, glewGetErrorString(err));
		assert(false);
	}
}
