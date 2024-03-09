#pragma once

#include <vector>
#include <filesystem>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "dllapi.h"

class Mesh
{
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	
	bool dynamic;

public:
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord; //uv (todo: w?)
	};
	std::vector<Vertex> vertices;

	std::vector<unsigned int> triangles;

	ENGINEGRAPHICS_API Mesh();
	ENGINEGRAPHICS_API ~Mesh();

	Mesh(const Mesh& cpy) = delete;
	Mesh operator=(const Mesh& cpy) = delete;
	ENGINEGRAPHICS_API Mesh(Mesh&& mov);
	ENGINEGRAPHICS_API Mesh& operator=(Mesh&& mov);

	ENGINEGRAPHICS_API void markDynamic();

	ENGINEGRAPHICS_API void uploadToGPU();
	ENGINEGRAPHICS_API void renderImmediate() const;

	//Loading functions
	ENGINEGRAPHICS_API bool load(const std::filesystem::path& path);

	//Primitives
	ENGINEGRAPHICS_API static Mesh createCube(float size);
	ENGINEGRAPHICS_API static Mesh createQuad0WH(float w, float h); //AA rect with one corner at (0,0) and the other at (w,h), normal Z+
};