#include "OpenGlMesh.hpp"

#include <ofbx.h>
#include <GL/glew.h>
#include "OpenGlRenderer.hpp"

OpenGlMesh::OpenGlMesh(const CMesh& src, bool dynamic) :
	VAO(0),
	VBO(0),
	EBO(0),
	nTriangles(0)
{
	auto usage = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, src.vertices.size() * sizeof(CMesh::Vertex), &src.vertices[0], usage);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, src.triangles.size() * sizeof(decltype(src.triangles)::value_type), &src.triangles[0], usage);
	nTriangles = src.triangles.size();

	//Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CMesh::Vertex), (void*)offsetof(CMesh::Vertex, position));
	//Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CMesh::Vertex), (void*)offsetof(CMesh::Vertex, normal));
	//UVs
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CMesh::Vertex), (void*)offsetof(CMesh::Vertex, texCoord));

	glBindVertexArray(0);
}

OpenGlMesh::~OpenGlMesh()
{
	if (VAO) glDeleteVertexArrays(1, &VAO);
	if (VBO) glDeleteBuffers(1, &VBO);
	if (EBO) glDeleteBuffers(1, &EBO);
}

void OpenGlMesh::updateFrom(const CMesh& src)
{
	updateFrom(src, true, true);
}

void OpenGlMesh::updateFrom(const CMesh& src, bool vertices, bool triangles)
{
	assert(*this);

	if (vertices)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, src.vertices.size() * sizeof(CMesh::Vertex), &src.vertices[0]);
	}

	if (triangles)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, src.triangles.size() * sizeof(decltype(src.triangles)::value_type), &src.triangles[0]);
	}
}

void OpenGlMesh::renderImmediate(Renderer*) const
{
	assert(*this);

	glColor4f(1, 1, 1, 1);

	//Draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, nTriangles, GL_UNSIGNED_INT, 0);

	//Clear state
	glBindVertexArray(0);
}

OpenGlMesh::OpenGlMesh() :
	VAO(0),
	VBO(0),
	EBO(0)
{
}

OpenGlMesh::OpenGlMesh(OpenGlMesh&& mov) :
	OpenGlMesh()
{
	*this = std::move(mov); //Defer
}

GMesh& OpenGlMesh::operator=(GMesh&& mov)
{
	*this = static_cast<OpenGlMesh&&>(mov);
	return *this;
}

OpenGlMesh& OpenGlMesh::operator=(OpenGlMesh&& mov)
{
	this->VAO = mov.VAO;
	this->VBO = mov.VBO;
	this->EBO = mov.EBO;
	this->nTriangles = mov.nTriangles;

	mov.VAO = 0;
	mov.VBO = 0;
	mov.EBO = 0;
	mov.nTriangles = 0;

	return *this;
}

OpenGlMesh::operator bool() const
{
	return VAO && VBO && EBO;
}
