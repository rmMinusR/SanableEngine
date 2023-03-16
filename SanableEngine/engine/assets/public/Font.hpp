#pragma once

#include <SDL_ttf.h>

#include "FileAsset.hpp"


class Font : public FileAsset
{
protected:
	virtual void loadInternal(MemoryManager*) override;
	virtual void unloadInternal(MemoryManager*) override;

	friend class Renderer;
	TTF_Font* handle;
	int size;
public:
	ENGINEASSETS_API Font(const std::filesystem::path&, int size);
};
