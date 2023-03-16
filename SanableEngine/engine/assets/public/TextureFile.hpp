#pragma once

#include "FileAsset.hpp"

struct SDL_Surface;

class TextureFile : public FileAsset
{
protected:
	virtual void loadInternal(MemoryManager*) override;
	virtual void unloadInternal(MemoryManager*) override;

	SDL_Surface* handle;
public:
	ENGINEASSETS_API TextureFile(const std::filesystem::path&);
};
