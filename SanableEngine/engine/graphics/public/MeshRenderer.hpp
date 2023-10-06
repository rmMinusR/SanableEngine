#pragma once

#include "Component.hpp"
#include "Mesh.hpp"
#include "dllapi.h"

class MeshRenderer : public Component, public IRenderable
{
	Mesh* mesh;
public:
	ENGINEGRAPHICS_API MeshRenderer(Mesh* mesh);
	ENGINEGRAPHICS_API ~MeshRenderer() = default;

	ENGINEGRAPHICS_API void Render(Renderer* renderer) override;
};
