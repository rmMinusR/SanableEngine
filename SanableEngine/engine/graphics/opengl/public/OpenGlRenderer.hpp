#pragma once

#include <glm/gtc/quaternion.hpp>
#include "Renderer.hpp"
#include "dllapi.h"
#include "OpenGlMesh.hpp"
#include "OpenGlTexture.hpp"

class OpenGlRenderer : public Renderer
{
private:
	SDL_GLContext sdlContext;

	OpenGlMesh unitQuad;
	OpenGlMesh dynQuad;
	OpenGlTexture fallbackTexture;

	Vector3f curCamPos;
	glm::quat curCamRot;

	void drawTextureInternal(const GTexture* tex, const Material* mat, Vector3f pos, Vector2f size, Rect<float> uvs, Color4<uint8_t> tintColor);
public:
	ENGINEOPENGL_API OpenGlRenderer(Window* owner, SDL_GLContext context);
	ENGINEOPENGL_API virtual ~OpenGlRenderer();

	ENGINEOPENGL_API virtual void activate() const override; // This renderer is contextual
	
	//These all require that no shader is active to properly render
	ENGINEOPENGL_API virtual void drawRect(Vector3f center, float w, float h, const Color4<uint8_t>& color) override;
	ENGINEOPENGL_API virtual void drawTextNonShadered(const Font& font, const std::wstring& text, Vector3f pos) override; //Assumes you have no shader active
	ENGINEOPENGL_API virtual void drawText(const Font& font, const Material& mat, const std::wstring& text, const Color4<uint8_t>& color) override; //Assumes you've already activated the material and set model matrix value
	ENGINEOPENGL_API virtual void drawTexture(const GTexture* tex, const Material* mat, Vector3f pos, float w, float h) override;
	ENGINEOPENGL_API virtual void drawSprite(const Sprite* spr, const Material* mat, Vector3f pos, float w, float h) override;
	ENGINEOPENGL_API virtual void drawSprite(const Sprite* spr, const Material* mat, Vector3f pos, float w, float h, Color4<uint8_t> tintColor) override;
	ENGINEOPENGL_API virtual void drawCMeshImmediate(const CMesh* mesh) override;
	
	ENGINEOPENGL_API virtual void setViewProjTranform(const glm::mat4&) override;
	ENGINEOPENGL_API virtual void setModelTransform(const glm::mat4&) override;
	ENGINEOPENGL_API virtual glm::mat4 getViewProjTranform() const override;
	ENGINEOPENGL_API virtual glm::mat4 getModelTransform() const override;
	ENGINEOPENGL_API virtual ShaderUniform::GlobalData getCurGlobalData() const override;

	ENGINEOPENGL_API virtual void setActiveShader(const ShaderProgram*) override;
	ENGINEOPENGL_API virtual void setMaterialFlags(const Material& material) override;

	ENGINEOPENGL_API virtual void beginFrame(const Camera& camSettings, Rect<float> viewport, Vector3<float> position, glm::quat rotation) override;
	ENGINEOPENGL_API virtual void endFrame() override;

	ENGINEOPENGL_API virtual void clear(Color4<float> color) override;

	[[nodiscard]] ENGINEOPENGL_API virtual GTexture* loadTexture(const std::filesystem::path& path) override;
	[[nodiscard]] ENGINEOPENGL_API virtual GTexture* newTexture(int width, int height, int nChannels, void* data) override;
	[[nodiscard]] ENGINEOPENGL_API virtual GMesh* newMesh(const CMesh& source) override;
	[[nodiscard]] ENGINEOPENGL_API virtual ShaderProgram* loadShaderProgram(const std::filesystem::path& path) override;

	ENGINEOPENGL_API virtual void errorCheck() const override;

	// Implementation-specific
	ENGINEOPENGL_API SDL_GLContext sdlHandle() const;
};
