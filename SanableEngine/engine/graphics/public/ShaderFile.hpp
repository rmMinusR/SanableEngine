#pragma once

#include <filesystem>
#include <GL/glew.h>
#include "dllapi.h"

class ShaderFile
{
public:
	enum class Type
	{
		Vertex = GL_VERTEX_SHADER,
		Fragment = GL_FRAGMENT_SHADER
	};

private:
	GLuint handle;
	std::filesystem::path path;
	Type type;

public:
	ENGINEGRAPHICS_API ShaderFile(const std::filesystem::path& path, Type type);
	ENGINEGRAPHICS_API ~ShaderFile();

	ENGINEGRAPHICS_API bool load();
	
	ENGINEGRAPHICS_API ShaderFile(ShaderFile&& mov);
	ENGINEGRAPHICS_API ShaderFile& operator=(ShaderFile&& mov);
	ShaderFile(const ShaderFile& cpy) = delete;
	ShaderFile& operator=(const ShaderFile& cpy) = delete;
};
