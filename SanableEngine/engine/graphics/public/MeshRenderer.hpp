#pragma once

#include "game/Component.hpp"
#include "dllapi.h"

class Mesh;
class Material;
class Renderer;

class MeshRenderer : public Component, public I3DRenderable
{
	Mesh* mesh;
	Material* material;
public:
	ENGINEGRAPHICS_API MeshRenderer(Mesh* mesh, Material* material);
	ENGINEGRAPHICS_API virtual ~MeshRenderer();

	ENGINEGRAPHICS_API Material* getMaterial() const override;
protected:
	ENGINEGRAPHICS_API void renderImmediate(Renderer* renderer) const override;
};
