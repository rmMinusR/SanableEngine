#include "Material.hpp"

#include "ShaderProgram.hpp"

Material::Material(ShaderProgram* shader) :
	shader(shader),
	group(Group::Opaque)
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

Material::Group Material::getGroup() const
{
	return group;
}

void Material::setGroup(Group group)
{
	this->group = group;
}

const ShaderProgram* Material::getShader() const
{
	return shader;
}

const ShaderUniform* Material::getUserUniform(const std::string& name) const
{
	for (const ShaderUniform& i : userConfigurable) if (i.name == name) return &i;
	return nullptr;
}

void Material::writeFlags(Renderer* context) const
{
	if (group == Group::Transparent)
	{
		glEnable(GL_BLEND);
		//TODO set blend func
	}
	else
	{
		glDisable(GL_BLEND);
	}
}

void Material::writeSharedUniforms(Renderer* context) const
{
	for (const ShaderUniform& uniform : sharedUniforms) uniform.tryBindShared(context);
	for (const ShaderUniform& uniform : userConfigurable) uniform.tryBindShared(context);
}

void Material::writeInstanceUniforms(Renderer* context, const I3DRenderable* target) const
{
	for (const ShaderUniform& uniform : instanceUniforms) uniform.tryBindInstanced(context, target);
}

void Material::writeInstanceUniforms(Renderer* context, const Widget* target) const
{
	for (const ShaderUniform& uniform : instanceUniforms) uniform.tryBindInstanced(context, target);
}

void Material::writeInstanceUniforms_generic(Renderer* context) const
{
	for (const ShaderUniform& uniform : instanceUniforms) uniform.tryBindInstanced_generic(context);
}