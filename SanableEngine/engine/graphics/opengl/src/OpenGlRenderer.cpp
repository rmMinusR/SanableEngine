#include "OpenGlRenderer.hpp"

#include <SDL_render.h>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Camera.hpp"
#include "Material.hpp"
#include "Font.hpp"
#include "Sprite.hpp"
#include "Window.hpp"
#include "OpenGlTexture.hpp"
#include "OpenGlShaderProgram.hpp"
#include "OpenGlMesh.hpp"
#include "OpenGlFramebuffer.hpp"
#include "GLContext.hpp"

OpenGlRenderer::OpenGlRenderer(Window* owner, SDL_GLContext context) :
	Renderer(owner),
	sdlContext(context)
{
	activate();

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
	if (sdlContext)
	{
		GLContext::release(sdlContext, this);
		sdlContext = nullptr;
	}
}

void OpenGlRenderer::activate() const
{
	owner->setActiveDrawTarget();
}

ShaderUniform::GlobalData OpenGlRenderer::getCurGlobalData() const
{
	ShaderUniform::GlobalData data;
	data.CameraPosition = curCamPos;
	data.ViewProjection = getViewProjTranform();
	return data;
}

void OpenGlRenderer::drawRect(Vector3f center, float w, float h, const Color4<uint8_t>& color)
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

void OpenGlRenderer::drawText(const Font& font, const Material& mat, const std::wstring& text, const Color4<uint8_t>& color)
{
	errorCheck();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	setActiveShader(mat.getShader());
	mat.getShader()->writeSharedUniforms(this, getCurGlobalData());
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
		
		unitQuad.renderImmediate(this);
		
		glPopMatrix();
		
		//Advance position
		relpos.x += glyph->getAdvance();
	}

	glPopMatrix();

	glDisable(GL_BLEND);
	errorCheck();
}

void OpenGlRenderer::drawTextureInternal(const GTexture* _tex, const Material* mat, Vector3f pos, Vector2f size, Rect<float> uvs, Color4<uint8_t> tintColor)
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

		mat->getShader()->writeSharedUniforms(this, getCurGlobalData());
		const ShaderUniform* transformUniform = mat->getUniform(ShaderUniform::ValueBinding::GeometryTransform);
		if (transformUniform)
		{
			glm::mat4 modelViewMatrix;
			glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(modelViewMatrix));
			transformUniform->write(modelViewMatrix); // Code smell...?
		}

		unitQuad.renderImmediate(this);
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

void OpenGlRenderer::drawSprite(const Sprite* spr, const Material* mat, Vector3f pos, float w, float h, Color4<uint8_t> tintColor)
{
	drawTextureInternal(spr->getTexture(), mat, pos, {w,h}, spr->getUVs(), tintColor);
}

void OpenGlRenderer::drawCMeshImmediate(const CMesh* mesh)
{
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < mesh->triangles.size(); i += 3)
	{
		const CMesh::Vertex& v0 = mesh->vertices[mesh->triangles[i + 0]];
		glTexCoord2f(v0.texCoord.x, v0.texCoord.y);
		glNormal3f(v0.normal.x, v0.normal.y, v0.normal.z);
		glVertex3f(v0.position.x, v0.position.y, v0.position.z);

		const CMesh::Vertex& v1 = mesh->vertices[mesh->triangles[i + 1]];
		glTexCoord2f(v1.texCoord.x, v1.texCoord.y);
		glNormal3f(v1.normal.x, v1.normal.y, v1.normal.z);
		glVertex3f(v1.position.x, v1.position.y, v1.position.z);

		const CMesh::Vertex& v2 = mesh->vertices[mesh->triangles[i + 2]];
		glTexCoord2f(v2.texCoord.x, v2.texCoord.y);
		glNormal3f(v2.normal.x, v2.normal.y, v2.normal.z);
		glVertex3f(v2.position.x, v2.position.y, v2.position.z);
	}
	glEnd();
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

glm::mat4 OpenGlRenderer::getViewProjTranform() const
{
	glm::mat4 mat;
	glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(mat));
	return mat;
}

glm::mat4 OpenGlRenderer::getModelTransform() const
{
	glm::mat4 mat;
	glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(mat));
	return mat;
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

void OpenGlRenderer::setMaterialFlags(const Material& material)
{
	if (material.getGroup() == Material::Group::Transparent)
	{
		glEnable(GL_BLEND);
		//TODO set blend func
	}
	else
	{
		glDisable(GL_BLEND);
	}
}

void OpenGlRenderer::beginFrame(const Camera& cam, Rect<float> viewport, Vector3<float> pos, glm::quat rot, const Framebuffer* framebuffer)
{
	if (framebuffer)
	{
		assert(dynamic_cast<const OpenGlFramebuffer*>(framebuffer) != nullptr);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<const OpenGlFramebuffer*>(framebuffer)->frameBuffer);
	}
	else
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	errorCheck();

	curCamPos = pos;
	curCamRot = rot;

	glm::mat4 projMat = cam.getMatrix(viewport);
	glm::mat4 viewMat = glm::identity<glm::mat4>();

	//Transform unless GUI - TODO move?
	if (cam.getMode() != Camera::Mode::GUI)
	{
		viewMat *= glm::mat4_cast(rot);
		viewMat = glm::translate(viewMat, (glm::vec3)-pos);
	}

	setViewProjTranform(projMat * viewMat);
}

void OpenGlRenderer::endFrame()
{
	// Nothing to do
	// Don't flip buffers, in case we're rendering to a texture
}

void OpenGlRenderer::clear(Color4<float> color)
{
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

Framebuffer* OpenGlRenderer::newFramebuffer(Vector2<int> initialSize, const Framebuffer::Settings& settings)
{
	return new OpenGlFramebuffer(this, initialSize, settings);
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

SDL_GLContext OpenGlRenderer::sdlHandle() const
{
	return sdlContext;
}
