#pragma once

#include <filesystem>


class DynamicModule
{
public:
	virtual ~DynamicModule();

	std::filesystem::path getPath() const;
	virtual void* getSymbol(const char* name) const = 0;

protected:
	DynamicModule(std::filesystem::path path);

	std::filesystem::path path;


	// No copying, no moving. You should be storing pointers to this anyway
	DynamicModule(const DynamicModule&) = delete;
	DynamicModule(DynamicModule&&) = delete;
	DynamicModule& operator=(const DynamicModule&) = delete;
	DynamicModule& operator=(DynamicModule&&) = delete;
};
