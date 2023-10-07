#include "MeshRenderer.hpp"

#include "Mesh.hpp"

#include <GL/glew.h>

MeshRenderer::MeshRenderer(Mesh* mesh) :
	mesh(mesh)
{
}

void MeshRenderer::Render(Renderer* renderer)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vector3f pos = gameObject->getTransform()->getPosition();
	glTranslatef(pos.x, pos.y, pos.z);

	mesh->render();
}
