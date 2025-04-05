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
#include "game/MeshRenderer.hpp"
#include "Font.hpp"
#include "Sprite.hpp"

Renderer::Renderer(Window* owner) :
	owner(owner)
{
}

Renderer::~Renderer()
{
}

OpenGlRenderer::OpenGlRenderer(Window* owner, SDL_GLContext context) :
	Renderer(owner),
	context(context)
{
	{
		CMesh cUnitQuad = CMesh::createUnitQuad(Rect<float>::fromMinMax({ 0,0 }, { 1,1 }));
		unitQuad = OpenGlMesh(cUnitQuad, false);
		dynQuad = OpenGlMesh(cUnitQuad, true);
	}

	{
		CTexture tmp(4, 4, 4);

		// Pink and black checkerboard
		constexpr uint8_t col_blk[4] = { 0, 0, 0, 255 };
		constexpr uint8_t col_mag[4] = { 255, 0, 255, 255 };
		for (int x = 0; x < tmp.getWidth(); ++x)
		{
			for (int y = 0; y < tmp.getWidth(); ++y)
			{
				memcpy(tmp.pixel(x, y), (x^y) ? &col_blk : &col_mag, 4);
			}
		}

		fallbackTexture = OpenGlTexture(this, tmp);
	}
}

OpenGlRenderer::~OpenGlRenderer()
{
	// TODO: Should we own the SDL_GLContext handle?
}

void OpenGlRenderer::activate() const
{
	Window::setActiveDrawTarget(getOwner());
}

void OpenGlRenderer::drawRect(Vector3f center, float w, float h, const SDL_Color& color)
{
	setActiveShader(nullptr);

	glBegin(GL_QUADS);
	glColor4f(color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f);
	glVertex3f(center.x-w/2, center.y-h/2, center.z);
	glVertex3f(center.x+w/2, center.y-h/2, center.z);
	glVertex3f(center.x+w/2, center.y+h/2, center.z);
	glVertex3f(center.x-w/2, center.y+h/2, center.z);
	glEnd();
}

void OpenGlRenderer::drawTextNonShadered(const Font& font, const std::wstring& text, Vector3f pos)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int i = 0; i < text.length(); ++i)
	{
		const RenderedGlyph* glyph = font.getGlyph(text[i], this);
		const GTexture* glyphTex = glyph->getTexture();
		
		drawTexture(
			glyphTex,
			nullptr,
			pos+Vector3f(glyph->getBearingX(), -glyph->getBearingY(), 0),
			glyphTex->getWidth(),
			glyphTex->getHeight()
		);
		
		//Advance position
		pos.x += glyph->getAdvance();
	}

	glDisable(GL_BLEND);
}

void OpenGlRenderer::drawText(const Font& font, const Material& mat, const std::wstring& text, const SDL_Color& color)
{
	errorCheck();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	setActiveShader(mat.getShader());
	mat.writeSharedUniforms(this);
	const ShaderUniform* uTextColor = mat.getUserUniform("textColor");
	if (uTextColor) uTextColor->write(glm::vec3(color.r, color.g, color.b)/255.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(0, font.getSize(), 0); //Text's default anchor is bottom-left of line; follow convention of using top-left corner to anchor
	
	Vector3f relpos;
	for (int i = 0; i < text.length(); ++i)
	{
		//TODO special case for CR, LF, EOL (aka CRLF)

		glPushMatrix();
		glTranslatef(relpos.x, relpos.y, 0); //Apply current render root location

		const RenderedGlyph* glyph = font.getGlyph(text[i], this);
		if (!glyph) glyph = font.getFallbackGlyph(this);
		
		const OpenGlTexture* glyphTex = static_cast<const OpenGlTexture*>(glyph->getTexture());

		glBindTexture(GL_TEXTURE_2D, glyphTex->id);
		glTranslatef(glyph->getBearingX(), -glyph->getBearingY(), 0); //Apply glyph's requested offset for texture
		glScalef(glyphTex->getWidth(), glyphTex->getHeight(), 1); //Apply glyph's requested size
		const ShaderUniform* transformUniform = mat.getUniform(ShaderUniform::ValueBinding::GeometryTransform);
		if (transformUniform)
		{
			glm::mat4 modelViewMatrix;
			glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(modelViewMatrix));
			transformUniform->write(modelViewMatrix); // Code smell...?
		}
		
		unitQuad.renderImmediate();
		
		glPopMatrix();
		
		//Advance position
		relpos.x += glyph->getAdvance();
	}

	glPopMatrix();

	glDisable(GL_BLEND);
	errorCheck();
}

void OpenGlRenderer::drawTextureInternal(const GTexture* _tex, const Material* mat, Vector3f pos, Vector2f size, Rect<float> uvs, SDL_Color tintColor)
{
	const OpenGlTexture* tex = static_cast<const OpenGlTexture*>(_tex);
	assert(tex);
	
	errorCheck();

	glBindTexture(GL_TEXTURE_2D, tex->id);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glScalef(size.x, size.y, 1);

	if (mat)
	{
		setActiveShader(mat->getShader());

		const ShaderUniform* uTintColor = mat->getUserUniform("tintColor");
		if (uTintColor) uTintColor->write(glm::vec4(tintColor.r, tintColor.g, tintColor.b, tintColor.a)/255.0f);

		const ShaderUniform* uUvMin = mat->getUserUniform("uvMin");
		if (uUvMin) uUvMin->write((glm::vec2)uvs.topLeft);
		
		const ShaderUniform* uUvMax = mat->getUserUniform("uvMax");
		if (uUvMax) uUvMax->write((glm::vec2)uvs.bottomRight());

		mat->writeSharedUniforms(this);
		const ShaderUniform* transformUniform = mat->getUniform(ShaderUniform::ValueBinding::GeometryTransform);
		if (transformUniform)
		{
			glm::mat4 modelViewMatrix;
			glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(modelViewMatrix));
			transformUniform->write(modelViewMatrix); // Code smell...?
		}

		unitQuad.renderImmediate();
	}
	else
	{
		//TODO implement color support
		glBegin(GL_QUADS);
		glTexCoord2f(uvs.      topLeft.x, uvs.      topLeft.y); glVertex3f(0, 0, 0);
		glTexCoord2f(uvs.bottomRight().x, uvs.      topLeft.y); glVertex3f(1, 0, 0);
		glTexCoord2f(uvs.bottomRight().x, uvs.bottomRight().y); glVertex3f(1, 1, 0);
		glTexCoord2f(uvs.      topLeft.x, uvs.bottomRight().y); glVertex3f(0, 1, 0);
		glEnd();
	}

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	errorCheck();
}

void OpenGlRenderer::drawTexture(const GTexture* tex, const Material* mat, Vector3f pos, float w, float h)
{
	if (!tex) tex = &fallbackTexture;
	drawTextureInternal(tex, mat, pos, {w,h}, Rect<float>::fromMinMax({0,0}, {1,1}), {255,255,255,255});
}

void OpenGlRenderer::drawSprite(const Sprite* spr, const Material* mat, Vector3f pos, float w, float h)
{
	drawTextureInternal(spr->getTexture(), mat, pos, {w,h}, spr->getUVs(), {255,255,255,255});
}

void OpenGlRenderer::drawSprite(const Sprite* spr, const Material* mat, Vector3f pos, float w, float h, SDL_Color tintColor)
{
	drawTextureInternal(spr->getTexture(), mat, pos, {w,h}, spr->getUVs(), tintColor);
}

void OpenGlRenderer::setViewProjTranform(const glm::mat4& mat)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(mat));
}

void OpenGlRenderer::setModelTransform(const glm::mat4& mat)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(mat));
}

void OpenGlRenderer::setActiveShader(const ShaderProgram* sourceUntyped)
{
	if (sourceUntyped)
	{
		const OpenGlShaderProgram* source = static_cast<const OpenGlShaderProgram*>(sourceUntyped);
		assert(source->handle);
		glUseProgram(source->handle);
	}
	else
	{
		glUseProgram(0); // Clear active shader
	}
}

GTexture* OpenGlRenderer::loadTexture(const std::filesystem::path& path)
{
	return new OpenGlTexture(this, CTexture::fromFile(path));
}

GTexture* OpenGlRenderer::newTexture(int width, int height, int nChannels, void* data)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, nChannels);
	return new OpenGlTexture(this, width, height, nChannels, data);
}

GMesh* OpenGlRenderer::newMesh(const CMesh& source)
{
	return new OpenGlMesh(source);
}

ShaderProgram* OpenGlRenderer::loadShaderProgram(const std::filesystem::path& path)
{
	return new OpenGlShaderProgram(path);
}

void OpenGlRenderer::errorCheck() const
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("GL error (code %u): %s\n", err, glewGetErrorString(err));
		assert(false);
	}
}
