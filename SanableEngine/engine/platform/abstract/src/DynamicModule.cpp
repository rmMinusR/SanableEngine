#include "DynamicModule.hpp"

DynamicModule::DynamicModule(std::filesystem::path path) :
	path(path)
{
}

DynamicModule::~DynamicModule()
{
}

std::filesystem::path DynamicModule::getPath() const
{
	return path;
}
