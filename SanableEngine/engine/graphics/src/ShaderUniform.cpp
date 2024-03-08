#include "ShaderUniform.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "Camera.hpp"
#include "MeshRenderer.hpp"
#include "ShaderProgram.hpp"
#include "gui/Widget.hpp"

const char* ShaderUniform::ValueBinding_getName(ValueBinding binding)
{
	switch (binding)
	{
#define _X(val) case ValueBinding::val: return #val;
	ValueBinding_VALUES_ALL
#undef _X

	default: return nullptr;
	}
}

ShaderUniform::ValueBinding ShaderUniform::ValueBinding_fromName(const std::string& name)
{
#define _X(val) if (std::strncmp(name.c_str(), #val, std::max(name.size(), strlen(#val))) == 0) return ValueBinding::val;
	ValueBinding_VALUES_ALL
#undef _X

	return (ValueBinding)-1;
}

void ShaderUniform::detectBinding()
{
	binding = ShaderUniform::ValueBinding_fromName(name);
	if (binding == ShaderUniform::ValueBinding::Invalid) binding = ShaderUniform::ValueBinding::Unbound;
}

ShaderUniform::ShaderUniform() :
	owner(nullptr),
	location(-1),
	name(""),
	objSize(0),
	dataType(0),
	binding(ShaderUniform::ValueBinding::Invalid)
{
}

ShaderUniform::ShaderUniform(ShaderProgram* owner, GLuint ownerHandle, int location) :
	owner(owner),
	location(location)
{
	constexpr size_t bufSz = 256;
	char buf[bufSz];
	GLsizei nRead;
	glGetActiveUniform(ownerHandle, location, bufSz, &nRead, &objSize, &dataType, buf);
	name = std::string(buf, nRead);
	location = glGetUniformLocation(ownerHandle, buf);
	detectBinding();
}

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
	default: //Unhandled binding
		assert(false);
		break;

	case ValueBinding::ViewProjection:
		glm::mat4 proj;
		glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(proj));
		write(proj);
		break;

	case ValueBinding::CameraPosition:
		write(Camera::getMain()->getGameObject()->getTransform()->getPosition());
		break;
	}
}

void ShaderUniform::tryBindInstanced(Renderer* context, const I3DRenderable* target) const
{
	if (getBindingStage() != BindingStage::BindInstanced) return;

	switch (binding)
	{
	default: //Unhandled binding
		assert(false);
		break;

	case ValueBinding::GeometryTransform:
		Transform* t = dynamic_cast<const Component*>(target)->getGameObject()->getTransform();
		write((glm::mat4)*t); //Should we be using GL matrices instead of GLM? Does it matter?
		break;
	}
}

void ShaderUniform::tryBindInstanced(Renderer* context, const Widget* target) const
{
	if (getBindingStage() != BindingStage::BindInstanced) return;

	switch (binding)
	{
	default: //Unhandled binding
		assert(false);
		break;

	case ValueBinding::GeometryTransform:
		write((glm::mat4)target->transform); //Should we be using GL matrices instead of GLM? Does it matter?
		break;
	}
}
