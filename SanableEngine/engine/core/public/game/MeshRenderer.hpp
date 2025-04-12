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
	ENGINECORE_API MeshRenderer(GMesh* mesh, Material* material);
	ENGINECORE_API virtual ~MeshRenderer();

	ENGINECORE_API const Material* getMaterial() const override;
protected:
	ENGINECORE_API virtual void loadModelTransform(Renderer* renderer) const override;
	ENGINECORE_API virtual void renderImmediate(Renderer* renderer) const override;
};
