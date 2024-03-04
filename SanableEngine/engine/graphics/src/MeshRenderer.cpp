#include "MeshRenderer.hpp"

#include "Mesh.hpp"

#include <GL/glew.h>

MeshRenderer::MeshRenderer(Mesh* mesh, Material* material) :
	mesh(mesh),
	material(material)
{
}

Material* MeshRenderer::getMaterial() const
{
	return material;
}

void MeshRenderer::renderImmediate(Renderer* renderer) const
{
	//Assumes correct material and shader have already been set up.
	mesh->renderImmediate();
	
	/*
	material->shader->activate();
	material->writeSharedUniforms(renderer);
	material->writeInstanceUniforms(renderer, this);
	mesh->renderImmediate();
	ShaderProgram::clear();
	*/
}

MeshRenderer::~MeshRenderer()
{
}
