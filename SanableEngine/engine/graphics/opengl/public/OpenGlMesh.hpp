#pragma once

#include <GL/glew.h>
#include "Mesh.hpp"
#include "dllapi.h"

class OpenGlMesh : public GMesh
{
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	size_t nTriangles;

public:
	ENGINEOPENGL_API OpenGlMesh();
	ENGINEOPENGL_API OpenGlMesh(const CMesh& src, bool dynamic = false);
	ENGINEOPENGL_API virtual ~OpenGlMesh();

	ENGINEOPENGL_API virtual void updateFrom(const CMesh& src) override;
	ENGINEOPENGL_API virtual void updateFrom(const CMesh& src, bool vertices, bool triangles) override;

	ENGINEOPENGL_API virtual void renderImmediate() const override;

	ENGINEOPENGL_API OpenGlMesh(OpenGlMesh&& mov);
	ENGINEOPENGL_API virtual GMesh& operator=(GMesh&& mov) override;
	ENGINEOPENGL_API OpenGlMesh& operator=(OpenGlMesh&& mov);

	ENGINEOPENGL_API virtual operator bool() const override;
};
