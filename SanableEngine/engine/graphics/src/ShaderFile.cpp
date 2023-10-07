#include "ShaderFile.hpp"

#include <fstream>
#include <cassert>

ShaderFile::ShaderFile(const std::filesystem::path& path, Type type) :
	handle(0),
	path(path),
	type(type)
{
}

bool ShaderFile::load()
{
	std::ifstream fin(path);
	if (!fin.good()) return false;

	//Check how much we need to allocate
	fin.seekg(0, std::ios_base::end);
	size_t size = fin.tellg();
	char* data = new char[size];
	
	//Read
	fin.seekg(0);
	fin.read(data, size);
	fin.close();

	//Compile shader
	handle = glCreateShader((GLenum)type);
	glShaderSource(handle, 1, &data, nullptr);
	glCompileShader(handle);

	//Check compile was good
	int status;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
	if (status != GL_NO_ERROR)
	{
		constexpr size_t logSz = 512;
		char errLog[logSz];
		glGetShaderInfoLog(handle, logSz, NULL, errLog);
		printf("Error compiling shader '%s':\n=========\n%s\n=========\n", path.u8string().c_str(), errLog);
		return false;
	}

	//All good!
	return true;
}

ShaderFile::~ShaderFile()
{
	if (handle)
	{
		glDeleteShader(handle);
	}
}

ShaderFile::ShaderFile(ShaderFile&& mov)
{
	*this = std::move(mov);
}

ShaderFile& ShaderFile::operator=(ShaderFile&& mov)
{
	this->path = mov.path;
	this->type = mov.type;

	this->handle = mov.handle;
	mov.handle = 0;
}
