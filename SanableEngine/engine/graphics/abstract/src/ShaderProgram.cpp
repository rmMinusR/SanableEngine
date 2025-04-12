#include "ShaderProgram.hpp"
#include "ShaderUniform.hpp"

#include <cassert>

void ShaderProgram::updateUniformBackrefs()
{
	for (size_t i = 0; i < getNumUniforms(); ++i) getUniform(i)->owner = this;
}

ShaderProgram::ShaderProgram(const std::filesystem::path& basePath) :
	basePath(basePath)
{
}

ShaderProgram::ShaderProgram()
{
}

ShaderProgram::~ShaderProgram()
{
}
