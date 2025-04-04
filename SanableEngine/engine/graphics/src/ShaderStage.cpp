#include "ShaderStage.hpp"

#include <fstream>
#include <cassert>

OpenGlShaderStage::OpenGlShaderStage():
	handle(0),
	path(""),
	type((Type)0)
{
}

OpenGlShaderStage::OpenGlShaderStage(const std::filesystem::path& path, Type type) :
	handle(0),
	path(path),
	type(type)
{
}

bool OpenGlShaderStage::load()
{
	std::ifstream fin(path);
	if (!fin.good()) return false;

	//Check how much we need to allocate
	fin.seekg(0, std::ios_base::end);
	size_t size = fin.tellg();
	char* data = new char[size];
	memset(data, 0, size);

	//Read
	fin.seekg(0);
	fin.read(data, size);
	fin.close();

	//Compile shader
	handle = glCreateShader((GLenum)type);
	glShaderSource(handle, 1, &data, nullptr);
	glCompileShader(handle);

	delete[] data;

	//Check compile was good
	int status;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		constexpr size_t logSz = 512;
		char errLog[logSz];
		glGetShaderInfoLog(handle, logSz, NULL, errLog);
		printf("Compiling shader '%s':\n%s\n", path.u8string().c_str(), errLog);
		return false;
	}

	//All good!
	return true;
}

void OpenGlShaderStage::unload()
{
	if (handle)
	{
		glDeleteShader(handle);
		handle = 0;
	}
}

OpenGlShaderStage::~OpenGlShaderStage()
{
	unload();
}

OpenGlShaderStage::OpenGlShaderStage(OpenGlShaderStage&& mov)
{
	*this = std::move(mov);
}

OpenGlShaderStage& OpenGlShaderStage::operator=(OpenGlShaderStage&& mov)
{
	if (this->handle) unload();

	this->handle = mov.handle;
	mov.handle = 0;

	this->path = mov.path;
	this->type = mov.type;

	return *this;
}
