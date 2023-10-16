#include "Texture.hpp"

#include <cassert>

#include <SDL_render.h>
#include <SDL_image.h>

#include "stb_image.h"

Texture::Texture()
{
}

Texture::Texture(const std::filesystem::path& path, SDL_GLContext ctx) :
	id(0),
	width(0),
	height(0),
	nChannels(0)
{
	//Load onto CPU
	void* data = stbi_load(path.u8string().c_str(), &width, &height, &nChannels, 0);
	assert(data);
	//cpu = IMG_Load(path.u8string().c_str());
	//assert(cpu);

	//Prep for GPU
	glGenTextures(1, &id);
	assert(id);

	//Send data to GPU
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	//Cleanup
	stbi_image_free(data);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

Texture::Texture(Texture&& mov)
{
	*this = std::move(mov); //Defer
}

Texture& Texture::operator=(Texture&& mov)
{
	this->id = mov.id;
	mov.id = 0;

	this->width = mov.width;
	this->height = mov.height;
	this->nChannels = mov.nChannels;
}
