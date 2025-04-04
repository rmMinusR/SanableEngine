#pragma once

#include <filesystem>
#include <GL/glew.h>
#include "dllapi.h"

class OpenGlShaderProgram;

class OpenGlShaderStage
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

	friend class OpenGlShaderProgram;

	OpenGlShaderStage();
public:
	ENGINEGRAPHICS_API OpenGlShaderStage(const std::filesystem::path& path, Type type);
	ENGINEGRAPHICS_API ~OpenGlShaderStage();

	ENGINEGRAPHICS_API bool load();
	ENGINEGRAPHICS_API void unload();
	
	ENGINEGRAPHICS_API OpenGlShaderStage(OpenGlShaderStage&& mov);
	ENGINEGRAPHICS_API OpenGlShaderStage& operator=(OpenGlShaderStage&& mov);
	OpenGlShaderStage(const OpenGlShaderStage& cpy) = delete;
	OpenGlShaderStage& operator=(const OpenGlShaderStage& cpy) = delete;
};
