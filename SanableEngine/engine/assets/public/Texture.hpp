#pragma once

#include "FileAsset.hpp"

#include <SDL_render.h>

struct SDL_Renderer;
struct SDL_Surface;

class Renderer;

class Texture : public FileAsset
{
protected:
	virtual void loadInternal(MemoryManager*) override;
	virtual void unloadInternal(MemoryManager*) override;

	friend class Renderer;
	SDL_Renderer* renderer;
	SDL_Surface* handle;
	SDL_Texture* view;
public:
	ENGINEASSETS_API Texture(const std::filesystem::path&, SDL_Renderer* renderer); //FIXME should use own renderer instead!
};
