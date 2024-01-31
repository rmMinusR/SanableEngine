#pragma once

#include "ShaderStage.hpp"
#include "ShaderUniform.hpp"

class ShaderProgram
{
	std::filesystem::path basePath;

	GLuint handle;

	static constexpr char* vertName = "vert.glsl";
	static constexpr char* fragName = "frag.glsl";

	std::vector<ShaderUniform> uniforms;

public:
	ENGINEGRAPHICS_API ShaderProgram(const std::filesystem::path& basePath);
	ENGINEGRAPHICS_API ~ShaderProgram();

	ENGINEGRAPHICS_API bool load();
	ENGINEGRAPHICS_API void unload();

	ENGINEGRAPHICS_API void activate() const;
	ENGINEGRAPHICS_API static void clear();

	ENGINEGRAPHICS_API const std::vector<ShaderUniform>& getUniforms() const;

	ENGINEGRAPHICS_API ShaderProgram(ShaderProgram&& mov);
	ENGINEGRAPHICS_API ShaderProgram& operator=(ShaderProgram&& mov);
	ShaderProgram(const ShaderProgram& cpy) = delete;
	ShaderProgram& operator=(const ShaderProgram& cpy) = delete;
};
