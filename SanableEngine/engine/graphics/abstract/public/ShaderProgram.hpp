#pragma once

#include <filesystem>
#include "ShaderUniform.hpp"

class ShaderProgram
{
protected:
	std::filesystem::path basePath;
	ENGINEGRAPHICS_API void updateUniformBackrefs();

	ENGINEGRAPHICS_API ShaderProgram(const std::filesystem::path& basePath);
	ENGINEGRAPHICS_API ShaderProgram();
public:
	ENGINEGRAPHICS_API virtual ~ShaderProgram();

	virtual bool load() = 0;
	virtual void unload() = 0;

	virtual size_t getNumUniforms() const = 0;
	virtual const ShaderUniform* getUniform(size_t index) const = 0;
	virtual ShaderUniform* getUniform(size_t index) = 0;

	ShaderProgram(ShaderProgram&& mov) = delete;
	ENGINEGRAPHICS_API virtual ShaderProgram& operator=(ShaderProgram&& mov) = 0;
	ShaderProgram(const ShaderProgram& cpy) = delete;
	ShaderProgram& operator=(const ShaderProgram& cpy) = delete;
};
