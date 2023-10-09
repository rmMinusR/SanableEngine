#include "Material.hpp"

#include "ShaderProgram.hpp"

Material::Material(ShaderProgram* shader) :
	shader(shader)
{
	for (const ShaderUniform& uniform : shader->getUniforms())
	{
		switch (uniform.getBindingStage())
		{
		case ShaderUniform::BindingStage::BindShared:
			sharedUniforms.push_back(uniform);
			break;
		case ShaderUniform::BindingStage::BindInstanced:
			instanceUniforms.push_back(uniform);
			break;
		case ShaderUniform::BindingStage::Unbound:
			userConfigurable.push_back(uniform);
			break;
		default:
			assert(false);
			break;
		}
	}
}

void Material::writeSharedUniforms(Renderer* context) const
{
	for (const ShaderUniform& uniform : sharedUniforms) uniform.tryBindShared(context);
	for (const ShaderUniform& uniform : userConfigurable) uniform.tryBindShared(context);
}

void Material::writeInstanceUniforms(Renderer* context, const MeshRenderer* meshRenderer) const
{
	for (const ShaderUniform& uniform : instanceUniforms) uniform.tryBindInstanced(context, meshRenderer);
}
