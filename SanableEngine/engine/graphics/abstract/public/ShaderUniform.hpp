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

	struct GlobalData
	{
		glm::vec3 CameraPosition;
		glm::mat4 ViewProjection;
	};

	struct ObjectData
	{
		glm::mat4 GeometryTransform;
	};

protected:
	ShaderProgram* owner;
	friend class ShaderProgram;
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

	virtual void tryBindShared(Renderer* renderer, const GlobalData* shared) const = 0;
	virtual void tryBindInstanced(Renderer* renderer, const ObjectData* object) const = 0;
};
