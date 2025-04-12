#pragma once

#include "ShaderUniform.hpp"
#include "dllapi.h"

class ShaderProgram;
class Renderer;
class I3DRenderable;
class Widget;

class OpenGlShaderUniform : public ShaderUniform
{
	int codeIndex;
	int location;
	std::string name;
	GLint objSize;
	GLenum dataType;
	void detectBinding();

public:
	ENGINEOPENGL_API OpenGlShaderUniform();
	ENGINEOPENGL_API OpenGlShaderUniform(ShaderProgram* owner, GLuint ownerHandle, int codeIndex);
	ENGINEOPENGL_API virtual ~OpenGlShaderUniform();

	ENGINEOPENGL_API virtual std::string_view getName() const override;

	ENGINEOPENGL_API virtual void write(float val) const override;
	ENGINEOPENGL_API virtual void write(glm::vec2 val) const override;
	ENGINEOPENGL_API virtual void write(glm::vec3 val) const override;
	ENGINEOPENGL_API virtual void write(glm::vec4 val) const override;
	ENGINEOPENGL_API virtual void write(glm::mat4 val) const override;

	ENGINEOPENGL_API virtual void tryBindShared(Renderer* context) const override;
	ENGINEOPENGL_API virtual void tryBindInstanced(Renderer* context, const I3DRenderable* target) const override;
	ENGINEOPENGL_API virtual void tryBindInstanced(Renderer* context, const Widget* target) const override;
private:
	bool tryBindInstanced_generic(Renderer* context) const; //Handles stuff common to both 3D objects and UI Widgets. Returns true if handled.
	friend class Material;
};
