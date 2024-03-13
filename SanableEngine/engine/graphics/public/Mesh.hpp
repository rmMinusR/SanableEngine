#pragma once

#include <vector>
#include <filesystem>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "dllapi.h"


class CMesh;
class GMesh;


//CPU-sided mesh
class CMesh
{
public:
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord; //uv (todo: w?)
	};
	std::vector<Vertex> vertices;

	std::vector<unsigned int> triangles;

	ENGINEGRAPHICS_API CMesh();
	ENGINEGRAPHICS_API CMesh(const std::filesystem::path& path);
	ENGINEGRAPHICS_API ~CMesh();

	//ENGINEGRAPHICS_API void renderImmediate() const;

	//Primitives
	//ENGINEGRAPHICS_API static CMesh createCube(float size);
};


class GMesh
{
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	size_t nTriangles;

public:
	ENGINEGRAPHICS_API GMesh();
	ENGINEGRAPHICS_API GMesh(const CMesh& src);
	ENGINEGRAPHICS_API ~GMesh();

	ENGINEGRAPHICS_API void renderImmediate() const;
};
