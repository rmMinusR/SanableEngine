#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <SDL_video.h>
#include <glm/glm.hpp>
#include "dllapi.h"
#include "math/Vector3.inl"
#include "math/Rect.inl"
#include "gui/Anchor2D.inl"
#include "Mesh.hpp"
#include "Texture.hpp"

class Application;
class Window;
class Font;
class Material;
class ShaderProgram;
class MeshRenderer;
class Sprite;
class UISprite;

struct SDL_Color;

class Renderer
{
protected:
	Window* owner;

	ENGINEGRAPHICS_API Renderer(Window* owner);
public:
	ENGINEGRAPHICS_API virtual ~Renderer();

	virtual void activate() const = 0; // Some renderers (such as OpenGL) are contextual
	inline Window* getOwner() const { return owner; }

	//These all require that no shader is active to properly render
	virtual void drawRect(Vector3f center, float w, float h, const SDL_Color& color) = 0;
	virtual void drawTextNonShadered(const Font& font, const std::wstring& text, Vector3f pos) = 0; //Assumes you have no shader active
	virtual void drawText(const Font& font, const Material& mat, const std::wstring& text, const SDL_Color& color) = 0; //Assumes you've already activated the material and set model matrix value
	virtual void drawTexture(const GTexture* tex, const Material* mat, Vector3f pos, float w, float h) = 0;
	virtual void drawSprite(const Sprite* spr, const Material* mat, Vector3f pos, float w, float h) = 0;
	virtual void drawSprite(const Sprite* spr, const Material* mat, Vector3f pos, float w, float h, SDL_Color tintColor) = 0;

	virtual void setViewProjTranform(const glm::mat4&) = 0;
	virtual void setModelTransform(const glm::mat4&) = 0;
	virtual void setActiveShader(const ShaderProgram* source) = 0;

	[[nodiscard]] virtual GTexture* loadTexture(const std::filesystem::path& path) = 0;
	[[nodiscard]] virtual GTexture* newTexture(int width, int height, int nChannels, void* data) = 0;
	[[nodiscard]] virtual GMesh* newMesh(const CMesh& source) = 0;
	[[nodiscard]] virtual ShaderProgram* loadShaderProgram(const std::filesystem::path& path) = 0;

	virtual void errorCheck() const = 0;
};

class OpenGlRenderer : public Renderer
{
private:
	SDL_GLContext context;

	OpenGlMesh unitQuad;
	OpenGlMesh dynQuad;
	OpenGlTexture fallbackTexture;

	void drawTextureInternal(const GTexture* tex, const Material* mat, Vector3f pos, Vector2f size, Rect<float> uvs, SDL_Color tintColor);
public:
	ENGINEGRAPHICS_API OpenGlRenderer(Window* owner, SDL_GLContext context);
	ENGINEGRAPHICS_API virtual ~OpenGlRenderer();

	ENGINEGRAPHICS_API virtual void activate() const override; // This renderer is contextual
	
	//These all require that no shader is active to properly render
	ENGINEGRAPHICS_API virtual void drawRect(Vector3f center, float w, float h, const SDL_Color& color) override;
	ENGINEGRAPHICS_API virtual void drawTextNonShadered(const Font& font, const std::wstring& text, Vector3f pos) override; //Assumes you have no shader active
	ENGINEGRAPHICS_API virtual void drawText(const Font& font, const Material& mat, const std::wstring& text, const SDL_Color& color) override; //Assumes you've already activated the material and set model matrix value
	ENGINEGRAPHICS_API virtual void drawTexture(const GTexture* tex, const Material* mat, Vector3f pos, float w, float h) override;
	ENGINEGRAPHICS_API virtual void drawSprite(const Sprite* spr, const Material* mat, Vector3f pos, float w, float h) override;
	ENGINEGRAPHICS_API virtual void drawSprite(const Sprite* spr, const Material* mat, Vector3f pos, float w, float h, SDL_Color tintColor) override;
	
	ENGINEGRAPHICS_API virtual void setViewProjTranform(const glm::mat4&) override;
	ENGINEGRAPHICS_API virtual void setModelTransform(const glm::mat4&) override;
	ENGINEGRAPHICS_API virtual void setActiveShader(const ShaderProgram* source) override;

	[[nodiscard]] ENGINEGRAPHICS_API virtual GTexture* loadTexture(const std::filesystem::path& path) override;
	[[nodiscard]] ENGINEGRAPHICS_API virtual GTexture* newTexture(int width, int height, int nChannels, void* data) override;
	[[nodiscard]] ENGINEGRAPHICS_API virtual GMesh* newMesh(const CMesh& source) override;
	[[nodiscard]] ENGINEGRAPHICS_API virtual ShaderProgram* loadShaderProgram(const std::filesystem::path& path) override;

	ENGINEGRAPHICS_API virtual void errorCheck() const override;
};
