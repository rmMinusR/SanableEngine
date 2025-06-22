#include "OpenGlShaderProgram.hpp"

#include <cassert>
#include <sstream>

#include "SyntheticTypeBuilder.hpp"

#include "OpenGlShaderUniform.hpp"
#include "OpenGlShaderStage.hpp"

const char* OpenGlShaderProgram::vertName = "vert.glsl";
const char* OpenGlShaderProgram::fragName = "frag.glsl";

OpenGlShaderProgram::OpenGlShaderProgram(const std::filesystem::path& basePath) :
	ShaderProgram(basePath),
	handle(0)
{
}

OpenGlShaderProgram::~OpenGlShaderProgram()
{
	unload();
}

OpenGlShaderProgram::OpenGlShaderProgram(OpenGlShaderProgram&& mov)
{
	*this = std::move(mov);
}

ShaderProgram& OpenGlShaderProgram::operator=(ShaderProgram&& mov)
{
	*this = std::move(static_cast<OpenGlShaderProgram&&>(mov));
	return *this;
}

OpenGlShaderProgram& OpenGlShaderProgram::operator=(OpenGlShaderProgram&& mov)
{
	if (this->handle) unload();
	this->handle = mov.handle;
	mov.handle = 0;

	this->uniforms = std::move(mov.uniforms);

	updateUniformBackrefs();

	return *this;
}

bool OpenGlShaderProgram::load()
{
	//Load dependencies
	OpenGlShaderStage vertShader(basePath/vertName, OpenGlShaderStage::Type::Vertex);
	OpenGlShaderStage fragShader(basePath/fragName, OpenGlShaderStage::Type::Fragment);
	bool stageLoadSuccess = vertShader.load();
	stageLoadSuccess &= fragShader.load();
	if (!stageLoadSuccess) return false;

	//Load and link self
	handle = glCreateProgram();
	glAttachShader(handle, vertShader.handle);
	glAttachShader(handle, fragShader.handle);
	glLinkProgram(handle);

	//Check link/compile was good
	int status;
	glGetProgramiv(handle, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		constexpr size_t logSz = 512;
		char errLog[logSz];
		glGetProgramInfoLog(handle, 512, NULL, errLog);
		printf("Linking shader program '%s':\n%s\n", basePath.u8string().c_str(), errLog);
		return false;
	}

	//Detect uniforms
	GLint nUniforms = 0;
	glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &nUniforms);
	for (int i = 0; i < nUniforms; ++i) uniforms.emplace_back(this, handle, i);

	// Build user uniform struct
	{
		std::stringstream uniformTypeName;
		uniformTypeName << "UserUniforms:" << basePath.string();

		SyntheticTypeBuilder userUniformsBuilder(uniformTypeName.str());
		for (const OpenGlShaderUniform& i : uniforms)
		{
			if (i.getBindingStage() == ShaderUniform::BindingStage::Unbound)
			{
				userUniformsBuilder.addField(*i.getGlmType(), i.getName(), MemberVisibility::All);
			}
		}
		userUniformStruct = userUniformsBuilder.finalize();
	}

	return true;
}

void OpenGlShaderProgram::unload()
{
	if (handle)
	{
		glDeleteProgram(handle);
		handle = 0;
	}
}

size_t OpenGlShaderProgram::getNumUniforms() const
{
	return uniforms.size();
}

const ShaderUniform* OpenGlShaderProgram::getUniform(size_t index) const
{
	return &uniforms[index];
}

ShaderUniform* OpenGlShaderProgram::getUniform(size_t index)
{
	return &uniforms[index];
}
