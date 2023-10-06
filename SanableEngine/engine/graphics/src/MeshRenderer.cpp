#include "MeshRenderer.hpp"

MeshRenderer::MeshRenderer(Mesh* mesh) :
	mesh(mesh)
{
}

void MeshRenderer::Render(Renderer* renderer)
{
	mesh->render();
}
