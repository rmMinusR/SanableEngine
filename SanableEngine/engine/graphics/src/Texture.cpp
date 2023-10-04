#include "Texture.hpp"

#include <cassert>

#include <SDL_render.h>
#include <SDL_image.h>

Texture::Texture(const std::filesystem::path& path, SDL_Renderer* renderer) :
	renderer(renderer),
	cpu(nullptr),
	gpu(nullptr)
{
	cpu = IMG_Load(path.u8string().c_str());
	assert(cpu);
	
	gpu = SDL_CreateTextureFromSurface(renderer, cpu);
	assert(gpu);
}

Texture::~Texture()
{
	SDL_DestroyTexture(gpu);
	gpu = nullptr;

	SDL_FreeSurface(cpu);
	cpu = nullptr;
}
