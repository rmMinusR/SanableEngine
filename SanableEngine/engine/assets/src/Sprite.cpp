#include "Sprite.hpp"

Sprite::Sprite(const std::filesystem::path& path, Texture* owner, SDL_Rect bounds) :
	FileAsset(path),
	owner(owner),
	bounds(bounds)
{
}

void Sprite::loadInternal()
{
	//TODO refcount
}

void Sprite::unloadInternal()
{
	//TODO refcount
}
