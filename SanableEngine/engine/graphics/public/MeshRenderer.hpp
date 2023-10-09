#pragma once

#include "Component.hpp"
#include "dllapi.h"

class Mesh;
class Material;
class Renderer;

class MeshRenderer : public Component, public IRenderable
{
	Mesh* mesh;
	Material* material;

	friend class Renderer;
public:
	ENGINEGRAPHICS_API MeshRenderer(Mesh* mesh, Material* material);
	ENGINEGRAPHICS_API ~MeshRenderer() = default;

	ENGINEGRAPHICS_API void Render(Renderer* renderer) override;
};
