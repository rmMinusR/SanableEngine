#include "MeshRenderer.hpp"

#include "Mesh.hpp"
#include "Renderer.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

MeshRenderer::MeshRenderer(GMesh* mesh, Material* material) :
	mesh(mesh),
	material(material)
{
}

const Material* MeshRenderer::getMaterial() const
{
	return material;
}

void MeshRenderer::loadModelTransform(Renderer* renderer) const
{
	renderer->loadTransform(*gameObject->getTransform());
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
