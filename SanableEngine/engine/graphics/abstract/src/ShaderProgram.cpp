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

void ShaderProgram::writeSharedUniforms(Renderer* context, const ShaderUniform::GlobalData& shared) const
{
	for (size_t i = 0; i < getNumUniforms(); ++i)
	{
		const ShaderUniform* uniform = getUniform(i);
		if (uniform->getBindingStage() == ShaderUniform::BindingStage::BindShared)
		{
			uniform->tryBindShared(context, &shared);
		}
	}
}

const TypeInfo* ShaderProgram::getUserUniformStruct() const
{
	return userUniformStruct.isValid() ? &userUniformStruct : nullptr;
}
