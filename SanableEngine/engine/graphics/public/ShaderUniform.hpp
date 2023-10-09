#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "dllapi.h"

class ShaderProgram;
class Renderer;
class MeshRenderer;

class ShaderUniform
{
public:
	enum class ValueBinding
	{
		Unbound = 0,

		//Shared
		ViewProjection,
		CameraPosition,

		__DIVINSTANCED,

		//Instanced
		Transform
	};

	enum class BindingStage
	{
		Unbound = 0,

		BindShared,
		BindInstanced
	};

//private:
	friend class ShaderProgram;
	ShaderProgram* owner;
	int location;
	std::string name;
	GLint objSize;
	GLenum dataType;
	ValueBinding binding = ValueBinding::Unbound;

public:
	ENGINEGRAPHICS_API ShaderProgram* getOwner() const;

	ENGINEGRAPHICS_API void write(float val) const;
	ENGINEGRAPHICS_API void write(glm::vec3 val) const;
	ENGINEGRAPHICS_API void write(glm::mat4 val) const;

	ENGINEGRAPHICS_API ValueBinding getBinding() const;
	ENGINEGRAPHICS_API BindingStage getBindingStage() const;

	ENGINEGRAPHICS_API void tryBindShared(Renderer* context) const;
	ENGINEGRAPHICS_API void tryBindInstanced(Renderer* context, const MeshRenderer* meshRenderer) const;
};
