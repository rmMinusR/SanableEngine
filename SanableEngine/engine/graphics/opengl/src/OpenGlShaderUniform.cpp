#include "OpenGlShaderUniform.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <TypeInfo.hpp>

#include "OpenGlShaderProgram.hpp"
#include "OpenGlRenderer.hpp"
#include "OpenGlTypes.hpp"

void OpenGlShaderUniform::detectBinding()
{
	binding = ShaderUniform::ValueBinding_fromName(name);
	if (binding == ShaderUniform::ValueBinding::Invalid) binding = ShaderUniform::ValueBinding::Unbound;
	bindingStage = ShaderUniform::BindingStage_fromBinding(binding);
}

OpenGlShaderUniform::OpenGlShaderUniform() :
	ShaderUniform(),
	location(-1),
	name(),
	objSize(0),
	dataType(0)
{
}

OpenGlShaderUniform::OpenGlShaderUniform(ShaderProgram* owner, GLuint ownerHandle, int codeIndex) :
	ShaderUniform(owner),
	codeIndex(codeIndex)
{
	constexpr size_t bufSz = 256;
	char buf[bufSz];
	GLsizei nRead;
	glGetActiveUniform(ownerHandle, codeIndex, bufSz, &nRead, &objSize, &dataType, buf);
	name = std::string(buf, nRead);
	location = glGetUniformLocation(ownerHandle, buf);
	glmType = ::getGlmType(dataType);
	detectBinding();
}

OpenGlShaderUniform::~OpenGlShaderUniform()
{
}

std::string_view OpenGlShaderUniform::getName() const
{
	return name;
}

void OpenGlShaderUniform::write(float val) const
{
	assert(dataType == GL_FLOAT); //Check data type
	glUniform1f(location, val);
}

void OpenGlShaderUniform::write(glm::vec2 val) const
{
	assert(dataType == GL_FLOAT_VEC2); //Check data type
	glUniform2f(location, val.x, val.y);
}

void OpenGlShaderUniform::write(glm::vec3 val) const
{
	assert(dataType == GL_FLOAT_VEC3); //Check data type
	glUniform3f(location, val.x, val.y, val.z);
}

void OpenGlShaderUniform::write(glm::vec4 val) const
{
	assert(dataType == GL_FLOAT_VEC4); //Check data type
	glUniform4f(location, val[0], val[1], val[2], val[3]);
}

void OpenGlShaderUniform::write(glm::mat4 val) const
{
	assert(dataType == GL_FLOAT_MAT4); //Check data type
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(val));
}

#define _X(name) case ValueBinding::name: write(shared->name); break;

void OpenGlShaderUniform::tryBindShared(Renderer* renderer, const GlobalData* shared) const
{
	if (getBindingStage() != BindingStage::BindShared) return;

	switch (binding)
	{
	default: //Unhandled binding
		assert(false);
		break;

		ValueBinding_VALUES_SHARED
	}
}

void OpenGlShaderUniform::tryBindInstanced(Renderer* renderer, const ObjectData* shared) const
{
	if (getBindingStage() != BindingStage::BindInstanced) return;

	switch (binding)
	{
	default: //Unhandled binding
		assert(false);
		break;

		ValueBinding_VALUES_INSTANCED
	}
}
