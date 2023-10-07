#pragma once

#include "Component.hpp"
#include "dllapi.h"

class Mesh;

class MeshRenderer : public Component, public IRenderable
{
	Mesh* mesh;
public:
	ENGINEGRAPHICS_API MeshRenderer(Mesh* mesh);
	ENGINEGRAPHICS_API ~MeshRenderer() = default;

	ENGINEGRAPHICS_API void Render(Renderer* renderer) override;
};
