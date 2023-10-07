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
	
public:
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord; //uv (todo: w?)
	};
	std::vector<Vertex> vertices;

	std::vector<unsigned int> faces;

	ENGINEGRAPHICS_API void uploadToGPU();
	ENGINEGRAPHICS_API void render();


	//Loading functions
	ENGINEGRAPHICS_API static bool load(Mesh& out, const std::filesystem::path& path);

	//Primitives
	ENGINEGRAPHICS_API static Mesh createCube(float size);
};