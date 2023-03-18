#pragma once

#include "FileAsset.hpp"

#include <SDL_rect.h>

class Texture;
class Renderer;

class Sprite : public FileAsset
{
protected:
	virtual void loadInternal() override;
	virtual void unloadInternal() override;

	friend class Renderer;
	Texture* owner;
public:
	ENGINEASSETS_API Sprite(const std::filesystem::path&, Texture* owner, SDL_Rect bounds);
	SDL_Rect bounds;
};
