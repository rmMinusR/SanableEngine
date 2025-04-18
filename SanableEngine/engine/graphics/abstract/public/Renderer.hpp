#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>
#include "dllapi.h"
#include "Color.inl"
#include "math/Vector3.inl"
#include "math/Rect.inl"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "ShaderUniform.hpp"

class Application;
class Window;
class Font;
class Camera;
class Material;
class ShaderProgram;
class MeshRenderer;
class Sprite;
class UISprite;
template<typename T> struct Color4;

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
	virtual void drawRect(Vector3f center, float w, float h, const Color4<uint8_t>& color) = 0;
	virtual void drawTextNonShadered(const Font& font, const std::wstring& text, Vector3f pos) = 0; //Assumes you have no shader active
	virtual void drawText(const Font& font, const Material& mat, const std::wstring& text, const Color4<uint8_t>& color) = 0; //Assumes you've already activated the material and set model matrix value
	virtual void drawTexture(const GTexture* tex, const Material* mat, Vector3f pos, float w, float h) = 0;
	virtual void drawSprite(const Sprite* spr, const Material* mat, Vector3f pos, float w, float h) = 0;
	virtual void drawSprite(const Sprite* spr, const Material* mat, Vector3f pos, float w, float h, Color4<uint8_t> tintColor) = 0;

	virtual void setViewProjTranform(const glm::mat4&) = 0;
	virtual void setModelTransform(const glm::mat4&) = 0;
	virtual glm::mat4 getViewProjTranform() const = 0;
	virtual glm::mat4 getModelTransform() const = 0;
	virtual ShaderUniform::GlobalData getCurGlobalData() const = 0;

	virtual void setActiveShader(const ShaderProgram* source) = 0;
	virtual void setMaterialFlags(const Material& material) = 0;

	virtual void beginFrame(const Camera& camSettings, Rect<float> viewport, Vector3<float> position, glm::quat rotation) = 0;
	virtual void endFrame() = 0;

	virtual void clear(Color4<float> color) = 0;

	[[nodiscard]] virtual GTexture* loadTexture(const std::filesystem::path& path) = 0;
	[[nodiscard]] virtual GTexture* newTexture(int width, int height, int nChannels, void* data) = 0;
	[[nodiscard]] virtual GMesh* newMesh(const CMesh& source) = 0;
	[[nodiscard]] virtual ShaderProgram* loadShaderProgram(const std::filesystem::path& path) = 0;

	virtual void errorCheck() const = 0;
};
