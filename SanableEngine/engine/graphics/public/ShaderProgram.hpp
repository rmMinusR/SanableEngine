#pragma once

#include "ShaderStage.hpp"

class ShaderProgram
{
public:
	class Uniform
	{
		friend class ShaderProgram;
		ShaderProgram* owner;
		int location;
		std::string name;
		GLint objSize;
		GLenum dataType;
	};

private:
	std::filesystem::path basePath;

	GLuint handle;

	static constexpr char* vertName = "vert.glsl";
	static constexpr char* fragName = "frag.glsl";

	std::vector<Uniform> uniforms;

public:
	ENGINEGRAPHICS_API ShaderProgram(const std::filesystem::path& basePath);
	ENGINEGRAPHICS_API ~ShaderProgram();

	ENGINEGRAPHICS_API bool load();
	ENGINEGRAPHICS_API void unload();

	ENGINEGRAPHICS_API void activate();
	ENGINEGRAPHICS_API static void clear();

	ENGINEGRAPHICS_API ShaderProgram(ShaderProgram&& mov);
	ENGINEGRAPHICS_API ShaderProgram& operator=(ShaderProgram&& mov);
	ShaderProgram(const ShaderProgram& cpy) = delete;
	ShaderProgram& operator=(const ShaderProgram& cpy) = delete;
};
