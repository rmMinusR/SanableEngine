#pragma once

#include <memory>
#include <filesystem>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "dllapi.h"


class FreetypeHandle
{
private:
	static std::weak_ptr<FreetypeHandle> instance;

	FT_Library ft;

public:
	ENGINEGRAPHICS_API static std::shared_ptr<FreetypeHandle> getInstance();

	FreetypeHandle();
	~FreetypeHandle();

	ENGINEGRAPHICS_API operator FT_Library() const;
	ENGINEGRAPHICS_API operator bool() const;
};


class Renderer;
class Texture;
class Font;

class RenderedGlyph
{
	Texture* texture;
	int bearingX;
	int bearingY;
	int advance;
	friend class Renderer;
	friend class Font;
public:
	ENGINEGRAPHICS_API RenderedGlyph();
	ENGINEGRAPHICS_API ~RenderedGlyph();

	ENGINEGRAPHICS_API operator bool() const;

	RenderedGlyph(const RenderedGlyph& cpy) = delete;
	RenderedGlyph operator=(const RenderedGlyph& cpy) = delete;
	ENGINEGRAPHICS_API RenderedGlyph(RenderedGlyph&& mov);
	ENGINEGRAPHICS_API RenderedGlyph& operator=(RenderedGlyph&& mov);
};

class Font
{
private:
	std::shared_ptr<FreetypeHandle> libHandle;
	FT_Face font;
	int size;
	friend class Renderer;

	mutable std::map<wchar_t, RenderedGlyph> cache; //TODO make per-Renderer
	//TODO memory pool of glyphs? Reallocation could be a good opportunity to test mover

	ENGINEGRAPHICS_API bool activateGlyph(wchar_t data, int flags) const; //Returns true if successful
	ENGINEGRAPHICS_API RenderedGlyph const* getGlyph(wchar_t data, Renderer* renderer) const; //Returns null if none found--use getFallbackGlyph if so.
	ENGINEGRAPHICS_API RenderedGlyph const* getFallbackGlyph(Renderer* renderer) const;
public:
	ENGINEGRAPHICS_API Font(const std::filesystem::path& path, int size);
	ENGINEGRAPHICS_API Font(const std::filesystem::path& path, int size, int index); //Some .ttf files have multiple fonts inside
	ENGINEGRAPHICS_API ~Font();
};
