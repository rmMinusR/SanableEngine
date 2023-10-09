#include "MeshRenderer.hpp"

#include "Mesh.hpp"
#include "ShaderProgram.hpp"

#include <GL/glew.h>

MeshRenderer::MeshRenderer(Mesh* mesh, Material* material) :
	mesh(mesh),
	material(material)
{
}

void MeshRenderer::Render(Renderer* renderer)
{
	renderer->delayedDrawMesh(this);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//Vector3f pos = gameObject->getTransform()->getPosition();
	//glTranslatef(pos.x, pos.y, pos.z);

	//shader->activate();
	//mesh->renderImmediate();
	//ShaderProgram::clear();
}
