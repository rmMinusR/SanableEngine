#pragma once

#include "Renderer.hpp"
#include "dllapi.h"

class WidgetAsWindow;

class WidgetAsWindow_RenderProxy : public Renderer
{
	Renderer* implementation;
	Framebuffer* defaultFramebuffer;

public:
	EDITORSHARED_API WidgetAsWindow_RenderProxy(Renderer* implementation, WidgetAsWindow* proxyOwner);
	EDITORSHARED_API virtual ~WidgetAsWindow_RenderProxy();

	EDITORSHARED_API void setDefaultFramebuffer(Framebuffer* framebuffer);

	EDITORSHARED_API virtual void activate() const override; // Some renderers (such as OpenGL) are contextual
	
	//These all require that no shader is active to properly render
	EDITORSHARED_API virtual void drawRect(Vector3f center, float w, float h, const Color4<uint8_t>& color) override;
	EDITORSHARED_API virtual void drawTextNonShadered(const Font& font, const std::wstring& text, Vector3f pos) override; //Assumes you have no shader active
	EDITORSHARED_API virtual void drawText(const Font& font, const Material& mat, const std::wstring& text, const Color4<uint8_t>& color) override; //Assumes you've already activated the material and set model matrix value
	EDITORSHARED_API virtual void drawTexture(const GTexture* tex, const Material* mat, Vector3f pos, float w, float h) override;
	EDITORSHARED_API virtual void drawSprite(const Sprite* spr, const Material* mat, Vector3f pos, float w, float h) override;
	EDITORSHARED_API virtual void drawSprite(const Sprite* spr, const Material* mat, Vector3f pos, float w, float h, Color4<uint8_t> tintColor) override;
	EDITORSHARED_API virtual void drawCMeshImmediate(const CMesh* mesh) override;

	EDITORSHARED_API virtual void setViewProjTranform(const glm::mat4&) override;
	EDITORSHARED_API virtual void setModelTransform(const glm::mat4&) override;
	EDITORSHARED_API virtual glm::mat4 getViewProjTranform() const override;
	EDITORSHARED_API virtual glm::mat4 getModelTransform() const override;
	EDITORSHARED_API virtual ShaderUniform::GlobalData getCurGlobalData() const override;

	EDITORSHARED_API virtual void setActiveShader(const ShaderProgram* source) override;
	EDITORSHARED_API virtual void setMaterialFlags(const Material& material) override;

	EDITORSHARED_API virtual void beginFrame(const Camera& camSettings, Rect<float> viewport, Vector3<float> position, glm::quat rotation, const Framebuffer* framebuffer = nullptr) override;
	EDITORSHARED_API virtual void endFrame() override;

	EDITORSHARED_API virtual void clear(Color4<float> color) override;

	[[nodiscard]] EDITORSHARED_API virtual GTexture* loadTexture(const std::filesystem::path& path) override;
	[[nodiscard]] EDITORSHARED_API virtual GTexture* newTexture(int width, int height, int nChannels, void* data) override;
	[[nodiscard]] EDITORSHARED_API virtual GMesh* newMesh(const CMesh& source) override;
	[[nodiscard]] EDITORSHARED_API virtual ShaderProgram* loadShaderProgram(const std::filesystem::path& path) override;
	[[nodiscard]] EDITORSHARED_API virtual Framebuffer* newFramebuffer(Vector2<int> initialSize, const Framebuffer::Settings& settings) override;

	EDITORSHARED_API virtual void errorCheck() const override;
};
