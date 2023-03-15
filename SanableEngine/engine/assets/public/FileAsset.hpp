#pragma once

#include "Asset.hpp"

#include <filesystem>

class FileAsset : public Asset
{
protected:
	std::filesystem::path path;

public:
	ENGINEASSETS_API FileAsset(const std::filesystem::path&);
};
