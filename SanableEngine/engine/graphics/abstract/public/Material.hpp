#pragma once

#include <vector>
#include "ShaderUniform.hpp"

class ShaderProgram;
class Renderer;
class MeshRenderer;
class I3DRenderable;
class Widget;

class Material
{
public:
	enum class Group : uint8_t
	{
		Opaque,
		Transparent
	};

private:
	ShaderProgram* shader;

	std::vector<const ShaderUniform*> sharedUniforms;
	std::vector<const ShaderUniform*> instanceUniforms;
	std::vector<const ShaderUniform*> userConfigurable;

	Group group;

public:
	ENGINEGRAPHICS_API Material(ShaderProgram* shader);

	ENGINEGRAPHICS_API Group getGroup() const;
	ENGINEGRAPHICS_API void setGroup(Group group);

	ENGINEGRAPHICS_API const ShaderProgram* getShader() const;

	ENGINEGRAPHICS_API const ShaderUniform* getUserUniform(const std::string& name) const;
	ENGINEGRAPHICS_API const ShaderUniform* getUniform(ShaderUniform::ValueBinding binding) const;

	ENGINEGRAPHICS_API void writeFlags(Renderer* context) const; //Typically need to run this after activating the shader
	ENGINEGRAPHICS_API void writeInstanceUniforms(Renderer* context, const ShaderUniform::ObjectData* data) const;
};
