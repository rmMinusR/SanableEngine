#include "Material.hpp"

#include "ShaderProgram.hpp"
#include "Renderer.hpp"

Material::Material(ShaderProgram* shader) :
	shader(shader),
	group(Group::Opaque)
{
	for (size_t i = 0; i < shader->getNumUniforms(); ++i)
	{
		const ShaderUniform* uniform = shader->getUniform(i);
		switch (uniform->getBindingStage())
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
	for (const ShaderUniform* i : userConfigurable) if (i->getName() == name) return i;
	return nullptr;
}

const ShaderUniform* Material::getUniform(ShaderUniform::ValueBinding binding) const
{
	for (const ShaderUniform* i : sharedUniforms) if (i->getBinding() == binding) return i;
	for (const ShaderUniform* i : instanceUniforms) if (i->getBinding() == binding) return i;
	return nullptr;
}

//void Material::writeUserUniforms(Renderer* context, const std::function<>& lookup) const
//{
//	for (const ShaderUniform* uniform : userConfigurable) uniform->tryBindShared(context, lookup);
//}

void Material::writeFlags(Renderer* context) const
{
	// Defer to main implementation
	context->setMaterialFlags(*this);
}

void Material::writeInstanceUniforms(Renderer* context, const ShaderUniform::ObjectData& data) const
{
	for (const ShaderUniform* uniform : instanceUniforms) uniform->tryBindInstanced(context, &data);
}
