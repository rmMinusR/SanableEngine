#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "dllapi.h"

class ShaderProgram;
class Renderer;
class I3DRenderable;
class Widget;

#define ValueBinding_VALUES_SHARED \
	_X(ViewProjection) \
	_X(CameraPosition)

#define ValueBinding_VALUES_INSTANCED \
	_X(GeometryTransform)

#define ValueBinding_VALUES_ALL _X(Unbound) ValueBinding_VALUES_SHARED ValueBinding_VALUES_INSTANCED

class ShaderUniform
{
public:
	enum class ValueBinding
	{
		Invalid = -1,
		Unbound = 0,

		//Shared
		__BEGIN_SHARED = Unbound,
#define _X(val) val,
		ValueBinding_VALUES_SHARED
#undef _X
		__END_SHARED,

		//Instanced
		__BEGIN_INSTANCED = __END_SHARED,
#define _X(val) val,
		ValueBinding_VALUES_INSTANCED
#undef _X
		__END_INSTANCED
	};
	static constexpr size_t ValueBinding_numInstanced = size_t(ValueBinding::__END_SHARED) - size_t(ValueBinding::__BEGIN_SHARED) - 1;
	static constexpr size_t ValueBinding_numShared = size_t(ValueBinding::__END_INSTANCED) - size_t(ValueBinding::__BEGIN_INSTANCED) - 1;
	static constexpr size_t ValueBinding_numValues = ValueBinding_numInstanced + ValueBinding_numShared;
	ENGINEGRAPHICS_API static const char* ValueBinding_getName(ValueBinding binding);
	ENGINEGRAPHICS_API static ValueBinding ValueBinding_fromName(const std::string& name);

	enum class BindingStage
	{
		Unbound = 0,

		BindShared,
		BindInstanced
	};
	ENGINEGRAPHICS_API static BindingStage BindingStage_fromBinding(ValueBinding binding);

protected:
	ShaderProgram* owner;
	ValueBinding binding;

	ENGINEGRAPHICS_API ShaderUniform();
	ENGINEGRAPHICS_API ShaderUniform(ShaderProgram* owner);

public:
	ENGINEGRAPHICS_API virtual ~ShaderUniform();
	ENGINEGRAPHICS_API ShaderProgram* getOwner() const;

	ENGINEGRAPHICS_API ValueBinding getBinding() const;
	ENGINEGRAPHICS_API BindingStage getBindingStage() const;

	virtual std::string_view getName() const = 0;

	virtual void write(float val) const = 0;
	virtual void write(glm::vec2 val) const = 0;
	virtual void write(glm::vec3 val) const = 0;
	virtual void write(glm::vec4 val) const = 0;
	virtual void write(glm::mat4 val) const = 0;

	virtual void tryBindShared(Renderer* context) const = 0;
	virtual void tryBindInstanced(Renderer* context, const I3DRenderable* target) const = 0;
	virtual void tryBindInstanced(Renderer* context, const Widget* target) const = 0;
};


class OpenGlShaderUniform : public ShaderUniform
{
	int codeIndex;
	int location;
	std::string name;
	GLint objSize;
	GLenum dataType;
	void detectBinding();

public:
	ENGINEGRAPHICS_API OpenGlShaderUniform();
	ENGINEGRAPHICS_API OpenGlShaderUniform(ShaderProgram* owner, GLuint ownerHandle, int codeIndex);
	ENGINEGRAPHICS_API virtual ~OpenGlShaderUniform();

	ENGINEGRAPHICS_API virtual std::string_view getName() const override;

	ENGINEGRAPHICS_API virtual void write(float val) const override;
	ENGINEGRAPHICS_API virtual void write(glm::vec2 val) const override;
	ENGINEGRAPHICS_API virtual void write(glm::vec3 val) const override;
	ENGINEGRAPHICS_API virtual void write(glm::vec4 val) const override;
	ENGINEGRAPHICS_API virtual void write(glm::mat4 val) const override;

	ENGINEGRAPHICS_API virtual void tryBindShared(Renderer* context) const override;
	ENGINEGRAPHICS_API virtual void tryBindInstanced(Renderer* context, const I3DRenderable* target) const override;
	ENGINEGRAPHICS_API virtual void tryBindInstanced(Renderer* context, const Widget* target) const override;
private:
	bool tryBindInstanced_generic(Renderer* context) const; //Handles stuff common to both 3D objects and UI Widgets. Returns true if handled.
	friend class Material;
};
