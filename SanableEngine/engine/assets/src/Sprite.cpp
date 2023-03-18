#include "Sprite.hpp"

#include <cassert>

#include "Texture.hpp"

Sprite::Sprite(const std::filesystem::path& path, Texture* spritesheet, SDL_Rect bounds) :
	FileAsset(path),
	spritesheet(spritesheet),
	bounds(bounds)
{
	assert(spritesheet);
}

void Sprite::loadInternal()
{
	require(spritesheet);
}

void Sprite::unloadInternal()
{
	release(spritesheet);
}
