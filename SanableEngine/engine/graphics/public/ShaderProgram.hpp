#pragma once

#include "ShaderStage.hpp"

class ShaderUniform;

class ShaderProgram
{
protected:
	std::filesystem::path basePath;

	ENGINEGRAPHICS_API ShaderProgram(const std::filesystem::path& basePath);
	ENGINEGRAPHICS_API ShaderProgram();
public:
	ENGINEGRAPHICS_API virtual ~ShaderProgram();

	virtual bool load() = 0;
	virtual void unload() = 0;

	virtual size_t getNumUniforms() const = 0;
	virtual const ShaderUniform* getUniform(size_t index) const = 0;

	ShaderProgram(ShaderProgram&& mov) = delete;
	ENGINEGRAPHICS_API virtual ShaderProgram& operator=(ShaderProgram&& mov) = 0;
	ShaderProgram(const ShaderProgram& cpy) = delete;
	ShaderProgram& operator=(const ShaderProgram& cpy) = delete;
};


class OpenGlRenderer;

class OpenGlShaderProgram : public ShaderProgram
{
	GLuint handle;

	ENGINEGRAPHICS_API static const char* vertName;
	ENGINEGRAPHICS_API static const char* fragName;

	std::vector<ShaderUniform> uniforms;

	friend class OpenGlRenderer;
	ENGINEGRAPHICS_API OpenGlShaderProgram(const std::filesystem::path& basePath);
public:
	ENGINEGRAPHICS_API virtual ~OpenGlShaderProgram();

	ENGINEGRAPHICS_API virtual bool load() override;
	ENGINEGRAPHICS_API virtual void unload() override;

	ENGINEGRAPHICS_API virtual size_t getNumUniforms() const override;
	ENGINEGRAPHICS_API virtual const ShaderUniform* getUniform(size_t index) const override;

	ENGINEGRAPHICS_API OpenGlShaderProgram(OpenGlShaderProgram&& mov);
	ENGINEGRAPHICS_API virtual ShaderProgram& operator=(ShaderProgram&& mov) override;
	ENGINEGRAPHICS_API OpenGlShaderProgram& operator=(OpenGlShaderProgram&& mov);
};
