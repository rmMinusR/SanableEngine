#pragma once

#include <SDL_rect.h>

#include "FileAsset.hpp"
#include "AssetUser.hpp"

class Texture;
class Renderer;

class Sprite : public FileAsset, public AssetUser
{
protected:
	virtual void loadInternal() override;
	virtual void unloadInternal() override;

	friend class Renderer;
	Texture* spritesheet;
public:
	ENGINEASSETS_API Sprite(const std::filesystem::path&, Texture* spritesheet, SDL_Rect bounds);
	SDL_Rect bounds;
};
