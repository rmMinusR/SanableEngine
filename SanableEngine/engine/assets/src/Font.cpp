#include "Font.hpp"

#include <cassert>

Font::Font(const std::filesystem::path& path, int size) :
	FileAsset(path),
	size(size)
{
}

void Font::loadInternal()
{
	handle = TTF_OpenFont(path.u8string().c_str(), size);
	assert(handle);
}

void Font::unloadInternal()
{
	TTF_CloseFont(handle);
	handle = nullptr;
}
