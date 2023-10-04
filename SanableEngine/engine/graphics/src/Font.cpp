#include "Font.hpp"

#include <cassert>

Font::Font(const std::filesystem::path& path, int size) :
	FileAsset(path),
	handle(nullptr),
	size(size)
{
	handle = TTF_OpenFont(path.u8string().c_str(), size);
	assert(handle);
}

Font::~Font()
{
	TTF_CloseFont(handle);
	handle = nullptr;
}
