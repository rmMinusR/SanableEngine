#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <EngineCoreReflectionHooks.hpp>

#include "dllapi.h"

class ShaderProgram;
class Renderer;
class I3DRenderable;

#define ValueBinding_VALUES_SHARED \
	_X(ViewProjection) \
	_X(CameraPosition)

#define ValueBinding_VALUES_INSTANCED \
	_X(GeometryTransform)

#define ValueBinding_VALUES_ALL _X(Unbound) ValueBinding_VALUES_SHARED ValueBinding_VALUES_INSTANCED

class ShaderUniform
{
	SANABLE_REFLECTION_HOOKS

public:
	enum class ValueBinding
	{
		Invalid = -1,
		Unbound = 0,

		//Shared
		#define _X(val) val,
		ValueBinding_VALUES_SHARED
		#undef _X

		__DIVINSTANCED,

		//Instanced
		#define _X(val) val,
		ValueBinding_VALUES_INSTANCED
		#undef _X

		__NUM_VALUES
	};
	static const char* ValueBinding_getName(ValueBinding binding);
	static ValueBinding ValueBinding_fromName(const std::string& name);

	enum class BindingStage
	{
		Unbound = 0,

		BindShared,
		BindInstanced
	};

private:
	ShaderProgram* owner;
	int location;
	std::string name;
	GLint objSize;
	GLenum dataType;
	ValueBinding binding;
	void detectBinding();

public:
	ENGINEGRAPHICS_API ShaderUniform();
	ENGINEGRAPHICS_API ShaderUniform(ShaderProgram* owner, GLuint ownerHandle, int location);

	ENGINEGRAPHICS_API ShaderProgram* getOwner() const;

	ENGINEGRAPHICS_API void write(float val) const;
	ENGINEGRAPHICS_API void write(glm::vec3 val) const;
	ENGINEGRAPHICS_API void write(glm::mat4 val) const;

	ENGINEGRAPHICS_API ValueBinding getBinding() const;
	ENGINEGRAPHICS_API BindingStage getBindingStage() const;

	ENGINEGRAPHICS_API void tryBindShared(Renderer* context) const;
	ENGINEGRAPHICS_API void tryBindInstanced(Renderer* context, const I3DRenderable* target) const;
};
