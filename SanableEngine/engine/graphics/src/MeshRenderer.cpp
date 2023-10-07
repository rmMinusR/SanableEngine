#include "MeshRenderer.hpp"

#include "Mesh.hpp"
#include "ShaderProgram.hpp"

#include <GL/glew.h>

MeshRenderer::MeshRenderer(Mesh* mesh, ShaderProgram* shader) :
	mesh(mesh),
	shader(shader)
{
}

void MeshRenderer::Render(Renderer* renderer)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vector3f pos = gameObject->getTransform()->getPosition();
	glTranslatef(pos.x, pos.y, pos.z);

	shader->activate();
	mesh->render();
	ShaderProgram::clear();
}
