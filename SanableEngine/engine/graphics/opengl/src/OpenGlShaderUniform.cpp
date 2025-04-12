#include "OpenGlShaderUniform.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "OpenGlShaderProgram.hpp"

void OpenGlShaderUniform::detectBinding()
{
	binding = ShaderUniform::ValueBinding_fromName(name);
	if (binding == ShaderUniform::ValueBinding::Invalid) binding = ShaderUniform::ValueBinding::Unbound;
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

void OpenGlShaderUniform::tryBindShared(Renderer* context) const
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
		write(CameraComponent::getMain()->getGameObject()->getTransform()->getPosition());
		break;
	}
}

void OpenGlShaderUniform::tryBindInstanced(Renderer* context, const I3DRenderable* target) const
{
	if (getBindingStage() != BindingStage::BindInstanced) return;

	if (tryBindInstanced_generic(context)) return;

	switch (binding)
	{
	default: //Unhandled binding
		assert(false);
		break;
	}
}

void OpenGlShaderUniform::tryBindInstanced(Renderer* context, const Widget* target) const
{
	if (getBindingStage() != BindingStage::BindInstanced) return;

	if (tryBindInstanced_generic(context)) return;

	switch (binding)
	{
	default: //Unhandled binding
		assert(false);
		break;
	}
}

bool OpenGlShaderUniform::tryBindInstanced_generic(Renderer* context) const
{
	switch (binding)
	{
	case ValueBinding::GeometryTransform:
		glm::mat4 mat; //Should we be using GL matrices instead of GLM? Does it matter?
		glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(mat));
		write(mat);
		return true;

	default: return false;
	}
}
