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
	ShaderProgram* shader;

	std::vector<ShaderUniform> sharedUniforms;
	std::vector<ShaderUniform> instanceUniforms;
	std::vector<ShaderUniform> userConfigurable;

public:
	ENGINEGRAPHICS_API Material(ShaderProgram* shader);

	ENGINEGRAPHICS_API const ShaderProgram* getShader() const;
	//ENGINEGRAPHICS_API const std::vector<ShaderUniform>& getParameters() const;

	ENGINEGRAPHICS_API const ShaderUniform* getUserUniform(const std::string& name) const;

	ENGINEGRAPHICS_API void writeSharedUniforms(Renderer* context) const;
	ENGINEGRAPHICS_API void writeInstanceUniforms(Renderer* context, const I3DRenderable* target) const;
	ENGINEGRAPHICS_API void writeInstanceUniforms(Renderer* context, const Widget* target) const;
	ENGINEGRAPHICS_API void writeInstanceUniforms_generic(Renderer* context) const; //Stateful: Reads GL matrix state. writeInstanceUniforms will call an equivalent of this.
};
