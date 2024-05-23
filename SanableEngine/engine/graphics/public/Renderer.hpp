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

struct SDL_Color;

class Renderer
{
private:
	Window* owner;
	SDL_GLContext context;

	GMesh dynQuad;
	GTexture fallbackTexture;

	void drawTextureInternal(const GTexture* tex, Vector3f pos, Vector2<float> size, Rect<float> uvs);
public:
	ENGINEGRAPHICS_API Renderer();
	ENGINEGRAPHICS_API Renderer(Window* owner, SDL_GLContext context);

	inline Window* getOwner() const { return owner; }
	
	//These all require that no shader is active to properly render
	ENGINEGRAPHICS_API void drawRect(Vector3f center, float w, float h, const SDL_Color& color);
	ENGINEGRAPHICS_API void drawTextNonShadered(const Font& font, const std::wstring& text, Vector3f pos); //Assumes you have no shader active
	ENGINEGRAPHICS_API void drawText(const Font& font, const Material& mat, const std::wstring& text, const SDL_Color& color); //Assumes you've already activated the material and set model matrix value
	ENGINEGRAPHICS_API void drawTexture(const GTexture* tex, Vector3f pos, float w, float h);
	ENGINEGRAPHICS_API void drawSprite(const Sprite* spr, Vector3f pos, float w, float h);

	ENGINEGRAPHICS_API void loadTransform(const glm::mat4&); //Makes no assumptions about which matrix is active or its state, just overwrites

	[[nodiscard]] ENGINEGRAPHICS_API GTexture* loadTexture(const std::filesystem::path& path);
	[[nodiscard]] ENGINEGRAPHICS_API GTexture* newTexture(int width, int height, int nChannels, void* data);
	[[nodiscard]] ENGINEGRAPHICS_API GTexture* renderFontGlyph(const Font& font);

	ENGINEGRAPHICS_API static void errorCheck(); //TODO de-static
};
