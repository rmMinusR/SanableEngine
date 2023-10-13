#pragma once

#include <vector>
#include <EngineCoreReflectionHooks.hpp>
#include "ShaderUniform.hpp"

class ShaderProgram;
class Renderer;
class MeshRenderer;
class I3DRenderable;

class Material
{
	SANABLE_REFLECTION_HOOKS
	Material();

	ShaderProgram* shader;

	std::vector<ShaderUniform> sharedUniforms;
	std::vector<ShaderUniform> instanceUniforms;
	std::vector<ShaderUniform> userConfigurable;

public:
	ENGINEGRAPHICS_API Material(ShaderProgram* shader);

	ENGINEGRAPHICS_API const ShaderProgram* getShader() const;
	//ENGINEGRAPHICS_API const std::vector<ShaderUniform>& getParameters() const;

	void writeSharedUniforms(Renderer* context) const;
	void writeInstanceUniforms(Renderer* context, const I3DRenderable* target) const;
};
