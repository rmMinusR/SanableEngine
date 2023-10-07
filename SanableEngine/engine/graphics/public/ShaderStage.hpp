#pragma once

#include <filesystem>
#include <GL/glew.h>
#include "dllapi.h"

class ShaderProgram;

class ShaderStage
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

	friend class ShaderProgram;

public:
	ENGINEGRAPHICS_API ShaderStage(const std::filesystem::path& path, Type type);
	ENGINEGRAPHICS_API ~ShaderStage();

	ENGINEGRAPHICS_API bool load();
	ENGINEGRAPHICS_API void unload();
	
	ENGINEGRAPHICS_API ShaderStage(ShaderStage&& mov);
	ENGINEGRAPHICS_API ShaderStage& operator=(ShaderStage&& mov);
	ShaderStage(const ShaderStage& cpy) = delete;
	ShaderStage& operator=(const ShaderStage& cpy) = delete;
};
