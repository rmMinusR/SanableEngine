#include "ShaderProgram.hpp"

#include <cassert>

ShaderProgram::ShaderProgram(const std::filesystem::path& basePath) :
	basePath(basePath),
	handle(0)
{
}

ShaderProgram::~ShaderProgram()
{
	unload();
}

ShaderProgram::ShaderProgram(ShaderProgram&& mov)
{
	*this = std::move(mov);
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& mov)
{
	if (this->handle) unload();
	this->handle = mov.handle;
	mov.handle = 0;

	return *this;
}

bool ShaderProgram::load()
{
	//Load dependencies
	ShaderStage vertShader(basePath/vertName, ShaderStage::Type::Vertex);
	ShaderStage fragShader(basePath/fragName, ShaderStage::Type::Fragment);
	if (!vertShader.load()) return false;
	if (!fragShader.load()) return false;

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

	//All good
	return true;
}

void ShaderProgram::unload()
{
	if (handle)
	{
		glDeleteProgram(handle);
		handle = 0;
	}
}

void ShaderProgram::activate()
{
	assert(handle);
	glUseProgram(handle);
}

void ShaderProgram::clear()
{
	glUseProgram(0);
}
