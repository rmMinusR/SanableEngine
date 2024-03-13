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

	ENGINEGRAPHICS_API CMesh(CMesh&& mov);
	ENGINEGRAPHICS_API CMesh& operator=(CMesh&& mov);
	CMesh(const CMesh& cpy) = delete;
	CMesh& operator=(const CMesh& cpy) = delete;
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

	ENGINEGRAPHICS_API GMesh(GMesh&& mov);
	ENGINEGRAPHICS_API GMesh& operator=(GMesh&& mov);
	GMesh(const GMesh& cpy) = delete;
	GMesh& operator=(const GMesh& cpy) = delete;
};
