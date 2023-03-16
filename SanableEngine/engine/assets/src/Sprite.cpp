#include "Sprite.hpp"

Sprite::Sprite(const std::filesystem::path& path, Texture* owner, SDL_Rect bounds) :
	FileAsset(path),
	owner(owner),
	bounds(bounds)
{
}

void Sprite::loadInternal(MemoryManager*)
{
	//TODO refcount
}

void Sprite::unloadInternal(MemoryManager*)
{
	//TODO refcount
}
