#pragma once

#include <vector>
#include "ShaderUniform.hpp"

class ShaderProgram;
class Renderer;
class MeshRenderer;

class Material
{
	ShaderProgram* shader;

	friend class Renderer;
	void writeSharedUniforms(Renderer* context) const;
	void writeInstanceUniforms(Renderer* context, const MeshRenderer* meshRenderer) const;

	std::vector<ShaderUniform> sharedUniforms;
	std::vector<ShaderUniform> instanceUniforms;
	std::vector<ShaderUniform> userConfigurable;

public:
	ENGINEGRAPHICS_API Material(ShaderProgram* shader);

	//ENGINEGRAPHICS_API const std::vector<ShaderUniform>& getParameters() const;
};
