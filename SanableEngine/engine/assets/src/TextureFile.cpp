#include "TextureFile.hpp"

#include <cassert>

#include <SDL_render.h>
#include <SDL_image.h>

TextureFile::TextureFile(const std::filesystem::path& path) :
	FileAsset(path),
	handle(nullptr)
{
}

void TextureFile::loadInternal(MemoryManager*)
{
	handle = IMG_Load(path.u8string().c_str());
	assert(handle);
}

void TextureFile::unloadInternal(MemoryManager*)
{
	SDL_FreeSurface(handle);
	handle = nullptr;
}
