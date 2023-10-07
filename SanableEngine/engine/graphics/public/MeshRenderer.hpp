#pragma once

#include "Component.hpp"
#include "dllapi.h"

class Mesh;
class ShaderProgram;

class MeshRenderer : public Component, public IRenderable
{
	Mesh* mesh;
	ShaderProgram* shader;
public:
	ENGINEGRAPHICS_API MeshRenderer(Mesh* mesh, ShaderProgram* shader);
	ENGINEGRAPHICS_API ~MeshRenderer() = default;

	ENGINEGRAPHICS_API void Render(Renderer* renderer) override;
};
