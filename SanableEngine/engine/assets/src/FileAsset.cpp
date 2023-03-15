#include "FileAsset.hpp"

#include <cassert>

FileAsset::FileAsset(const std::filesystem::path& path) :
	path(path)
{
	assert(std::filesystem::exists(path));
}
