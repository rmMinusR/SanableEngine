#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <SDL_video.h>
#include <glm/glm.hpp>
#include "dllapi.h"
#include "Vector3.inl"
#include "Mesh.hpp"

class Application;
class Window;
class Font;
class Texture;
class Material;
class ShaderProgram;
class MeshRenderer;

struct SDL_Color;

class Renderer
{
private:
	Window* owner;
	SDL_GLContext context;

	Mesh dynQuad;

public:
	ENGINEGRAPHICS_API Renderer();
	ENGINEGRAPHICS_API Renderer(Window* owner, SDL_GLContext context);

	inline Window* getOwner() const { return owner; }
	
	//These all require that no shader is active to properly render
	ENGINEGRAPHICS_API void drawRect(Vector3f center, float w, float h, const SDL_Color& color);
	ENGINEGRAPHICS_API void drawTextNonShadered(const Font& font, const std::wstring& text, Vector3f pos); //Assumes you have no shader active
	ENGINEGRAPHICS_API void drawText(const Font& font, const Material& mat, const std::wstring& text); //Assumes you've already activated the material and set model matrix value
	ENGINEGRAPHICS_API void drawTexture(const Texture& tex, int x, int y); //Obsolete
	ENGINEGRAPHICS_API void drawTexture(const Texture* tex, Vector3f pos, float w, float h);

	ENGINEGRAPHICS_API void loadTransform(const glm::mat4&); //Makes no assumptions about which matrix is active or its state, just overwrites

	[[nodiscard]] ENGINEGRAPHICS_API Texture* loadTexture(const std::filesystem::path& path);
	[[nodiscard]] ENGINEGRAPHICS_API Texture* newTexture(int width, int height, int nChannels, void* data);
	[[nodiscard]] ENGINEGRAPHICS_API Texture* renderFontGlyph(const Font& font);

	ENGINEGRAPHICS_API static void errorCheck(); //TODO de-static
};
