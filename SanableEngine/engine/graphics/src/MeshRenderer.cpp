#include "MeshRenderer.hpp"

#include "Mesh.hpp"

#include <GL/glew.h>

MeshRenderer::MeshRenderer(Mesh* mesh, Material* material) :
	mesh(mesh),
	material(material)
{
}

void MeshRenderer::Render(Renderer* renderer)
{
	renderer->delayedDrawMesh(this);
	
	/*
	material->shader->activate();
	material->writeSharedUniforms(renderer);
	material->writeInstanceUniforms(renderer, this);
	mesh->renderImmediate();
	ShaderProgram::clear();
	*/
}
