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
	ENGINEOPENGL_API OpenGlShaderStage(const std::filesystem::path& path, Type type);
	ENGINEOPENGL_API ~OpenGlShaderStage();

	ENGINEOPENGL_API bool load();
	ENGINEOPENGL_API void unload();
	
	ENGINEOPENGL_API OpenGlShaderStage(OpenGlShaderStage&& mov);
	ENGINEOPENGL_API OpenGlShaderStage& operator=(OpenGlShaderStage&& mov);
	OpenGlShaderStage(const OpenGlShaderStage& cpy) = delete;
	OpenGlShaderStage& operator=(const OpenGlShaderStage& cpy) = delete;
};
