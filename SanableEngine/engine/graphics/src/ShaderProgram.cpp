#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram(const std::filesystem::path& basePath) :
	vertShader(basePath/vertName, ShaderStage::Type::Vertex),
	fragShader(basePath/fragName, ShaderStage::Type::Fragment),
	basePath(basePath),
	handle(0)
{
}

ShaderProgram::~ShaderProgram()
{
	unload();
}

bool ShaderProgram::load()
{
	//Load dependencies
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
	if (status != GL_NO_ERROR)
	{
		constexpr size_t logSz = 512;
		char errLog[logSz];
		glGetProgramInfoLog(handle, 512, NULL, errLog);
		printf("Error linking shader '%s':\n=========\n%s\n=========\n", basePath.u8string().c_str(), errLog);
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

	vertShader.unload();
	fragShader.unload();
}
