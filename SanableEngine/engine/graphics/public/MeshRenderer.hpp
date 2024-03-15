#pragma once

#include "game/Component.hpp"
#include "dllapi.h"

class GMesh;
class Material;
class Renderer;

class MeshRenderer : public Component, public I3DRenderable
{
	GMesh* mesh;
	Material* material;
public:
	ENGINEGRAPHICS_API MeshRenderer(GMesh* mesh, Material* material);
	ENGINEGRAPHICS_API virtual ~MeshRenderer();

	ENGINEGRAPHICS_API const Material* getMaterial() const override;
protected:
	ENGINEGRAPHICS_API virtual void loadModelTransform(Renderer* renderer) const override;
	ENGINEGRAPHICS_API virtual void renderImmediate(Renderer* renderer) const override;
};
