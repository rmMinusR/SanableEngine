#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <SDL_video.h>
#include <glm/glm.hpp>
#include "dllapi.h"
#include "Vector3.inl"

class EngineCore;
class Window;
class Font;
class Texture;
class Mesh;
class Material;
class ShaderProgram;
class MeshRenderer;

struct SDL_Color;

class Renderer
{
private:
	Window* owner;
	SDL_GLContext context;

public:
	ENGINEGRAPHICS_API Renderer();
	ENGINEGRAPHICS_API Renderer(Window* owner, const SDL_GLContext& context);

	inline Window* getOwner() const { return owner; }
	
	ENGINEGRAPHICS_API void drawRect(Vector3f center, float w, float h, const SDL_Color& color);
	ENGINEGRAPHICS_API void drawText(const Font& font, const SDL_Color& color, const std::wstring& text, Vector3f pos, bool highQuality = false);
	ENGINEGRAPHICS_API void drawTexture(const Texture& tex, int x, int y);

private:
	friend class EngineCore;
	void beginFrame();
	void finishFrame();
	
	friend class MeshRenderer;
	void delayedDrawMesh(const MeshRenderer* meshRenderer);

	//Shader pass grouping mechanism
	std::unordered_map<
		const ShaderProgram*, //Group by shader
		std::unordered_map<
			const Material*, //Then by material
			std::vector<const MeshRenderer*> //FIXME coupling
		>
	> bufferedMeshRenderCommands;
	void processMeshBuffer();
};
