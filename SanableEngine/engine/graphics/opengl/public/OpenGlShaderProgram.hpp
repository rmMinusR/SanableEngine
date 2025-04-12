#pragma once

#include "ShaderProgram.hpp"
#include "dllapi.h"
#include "OpenGlShaderUniform.hpp"

class OpenGlRenderer;

class OpenGlShaderProgram : public ShaderProgram
{
	GLuint handle;

	ENGINEOPENGL_API static const char* vertName;
	ENGINEOPENGL_API static const char* fragName;

	std::vector<OpenGlShaderUniform> uniforms;

	friend class OpenGlRenderer;
	ENGINEOPENGL_API OpenGlShaderProgram(const std::filesystem::path& basePath);
public:
	ENGINEOPENGL_API virtual ~OpenGlShaderProgram();

	ENGINEOPENGL_API virtual bool load() override;
	ENGINEOPENGL_API virtual void unload() override;

	ENGINEOPENGL_API virtual size_t getNumUniforms() const override;
	ENGINEOPENGL_API virtual const ShaderUniform* getUniform(size_t index) const override;
	ENGINEOPENGL_API virtual ShaderUniform* getUniform(size_t index) override;

	ENGINEOPENGL_API OpenGlShaderProgram(OpenGlShaderProgram&& mov);
	ENGINEOPENGL_API virtual ShaderProgram& operator=(ShaderProgram&& mov) override;
	ENGINEOPENGL_API OpenGlShaderProgram& operator=(OpenGlShaderProgram&& mov);
};
