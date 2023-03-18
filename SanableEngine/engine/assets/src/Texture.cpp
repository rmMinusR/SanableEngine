#include "Texture.hpp"

#include <cassert>

#include <SDL_render.h>
#include <SDL_image.h>

Texture::Texture(const std::filesystem::path& path, SDL_Renderer* renderer) :
	FileAsset(path),
	renderer(renderer),
	handle(nullptr),
	view(nullptr)
{
}

void Texture::loadInternal()
{
	handle = IMG_Load(path.u8string().c_str());
	assert(handle);

	view = SDL_CreateTextureFromSurface(renderer, handle);
	assert(view);
}

void Texture::unloadInternal()
{
	SDL_DestroyTexture(view);
	view = nullptr;

	SDL_FreeSurface(handle);
	handle = nullptr;
}
