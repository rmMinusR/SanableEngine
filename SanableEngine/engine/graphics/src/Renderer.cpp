#include "Renderer.hpp"

#include <SDL_render.h>
#include <SDL_pixels.h>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
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
	dynQuad = GMesh(CMesh::createQuad0WH(1, 1), true);
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

void Renderer::drawTextNonShadered(const Font& font, const std::wstring& text, Vector3f pos)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int i = 0; i < text.length(); ++i)
	{
		const RenderedGlyph* glyph = font.getGlyph(text[i], this);
		
		drawTexture(
			glyph->texture,
			pos+Vector3f(glyph->bearingX, -glyph->bearingY, 0),
			glyph->texture->width,
			glyph->texture->height
		);
		
		//Advance position
		pos.x += glyph->advance / 64.0f;
	}

	glDisable(GL_BLEND);
}

void Renderer::drawText(const Font& font, const Material& mat, const std::wstring& text)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(0, font.size, 0); //Text's default anchor is bottom-left of line; follow convention of using top-left corner to anchor
	
	Vector3f relpos;
	for (int i = 0; i < text.length(); ++i)
	{
		//TODO special case for CR, LF, EOL (aka CRLF)

		glPushMatrix();
		glTranslatef(relpos.x, relpos.y, 0); //Apply current render root location

		const RenderedGlyph* glyph = font.getGlyph(text[i], this);
		if (!glyph) glyph = font.getFallbackGlyph(this);
		
		glBindTexture(GL_TEXTURE_2D, glyph->texture->id);
		glTranslatef(glyph->bearingX, -glyph->bearingY, 0); //Apply glyph's requested offset for texture
		glScalef(glyph->texture->width, glyph->texture->height, 1); //Apply glyph's requested size
		mat.writeInstanceUniforms_generic(this); //Refresh ModelView. TODO: inefficient, don't refresh everything else

		dynQuad.renderImmediate();
		
		glPopMatrix();
		
		//Advance position
		relpos.x += glyph->advance / 64.0f;
	}

	glPopMatrix();

	glDisable(GL_BLEND);
}

void Renderer::drawTexture(const GTexture& tex, int x, int y)
{
	ShaderProgram::clear();
	drawTexture(&tex, Vector3f(x, y, 0), tex.width, tex.height);
}

void Renderer::drawTexture(const GTexture* tex, Vector3f pos, float w, float h)
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

void Renderer::loadTransform(const glm::mat4& mat)
{
	glLoadMatrixf(glm::value_ptr(mat));
}

GTexture* Renderer::loadTexture(const std::filesystem::path& path)
{
	return new GTexture(this, CTexture(path));
}

GTexture* Renderer::newTexture(int width, int height, int nChannels, void* data)
{
	return new GTexture(this, width, height, nChannels, data);
}

GTexture* Renderer::renderFontGlyph(const Font& font)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Disable byte-alignment restriction: OpenGL textures have 4-byte align and size, but here we're grayscale
	GTexture* out = new GTexture(
		this,
		font.font->glyph->bitmap.width,
		font.font->glyph->bitmap.rows,
		1,
		font.font->glyph->bitmap.buffer
	);
	return out;
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
