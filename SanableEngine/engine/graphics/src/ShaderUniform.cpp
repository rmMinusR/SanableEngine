#include "ShaderUniform.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "Camera.hpp"
#include "MeshRenderer.hpp"

ShaderProgram* ShaderUniform::getOwner() const
{
	return owner;
}

void ShaderUniform::write(float val) const
{
	assert(dataType == GL_FLOAT); //Check data type
	glUniform1f(location, val);
}

void ShaderUniform::write(glm::vec3 val) const
{
	assert(dataType == GL_FLOAT_VEC3); //Check data type
	glUniform3f(location, val.x, val.y, val.z);
}

void ShaderUniform::write(glm::mat4 val) const
{
	assert(dataType == GL_FLOAT_MAT4); //Check data type
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(val));
}

ShaderUniform::ValueBinding ShaderUniform::getBinding() const
{
	return binding;
}

ShaderUniform::BindingStage ShaderUniform::getBindingStage() const
{
	if (binding == ValueBinding::Unbound) return BindingStage::Unbound;
	else if ((int)binding > (int)ValueBinding::__DIVINSTANCED) return BindingStage::BindInstanced;
	else return BindingStage::BindShared;
}

void ShaderUniform::tryBindShared(Renderer* context) const
{
	if (getBindingStage() != BindingStage::BindShared) return;

	switch (binding)
	{
	case ValueBinding::ViewProjection:
		glm::mat4 proj;
		glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(proj));
		write(proj);
		break;

	case ValueBinding::CameraPosition:
		write(Camera::getMain()->getGameObject()->getTransform()->getPosition());
		break;

	default: //Unhandled binding
		assert(false);
		break;
	}
}

void ShaderUniform::tryBindInstanced(Renderer* context, const MeshRenderer* meshRenderer) const
{
	if (getBindingStage() != BindingStage::BindInstanced) return;

	switch (binding)
	{
	case ValueBinding::Transform:
		write((glm::mat4)*meshRenderer->getGameObject()->getTransform()); //Should we be using GL matrices instead of GLM? Does it matter?
		break;

	default: //Unhandled binding
		assert(false);
		break;
	}
}
