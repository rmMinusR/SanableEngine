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

class Font
{
private:
	std::shared_ptr<FreetypeHandle> libHandle;
	FT_Face font;
	friend class Renderer;

	class CachedGlyph
	{
		Texture* texture;
		int bearingX;
		int bearingY;
		int advance;
		friend class Renderer;
		friend class Font;
	public:
		ENGINEGRAPHICS_API CachedGlyph();
		ENGINEGRAPHICS_API ~CachedGlyph();

		ENGINEGRAPHICS_API operator bool() const;

		CachedGlyph(const CachedGlyph& cpy) = delete;
		CachedGlyph operator=(const CachedGlyph& cpy) = delete;
		ENGINEGRAPHICS_API CachedGlyph(CachedGlyph&& mov);
		ENGINEGRAPHICS_API CachedGlyph operator=(CachedGlyph&& mov);
	};
	mutable std::map<wchar_t, CachedGlyph> cache; //TODO make per-Renderer
	//TODO memory pool of glyphs? Reallocation could be a good opportunity to test mover

	ENGINEGRAPHICS_API bool activateGlyph(wchar_t data, int flags) const; //Returns true if successful
	ENGINEGRAPHICS_API CachedGlyph const* getGlyph(wchar_t data, Renderer* renderer) const;
public:
	ENGINEGRAPHICS_API Font(const std::filesystem::path& path, float size);
	ENGINEGRAPHICS_API Font(const std::filesystem::path& path, float size, int index); //Some .ttf files have multiple fonts inside
	ENGINEGRAPHICS_API ~Font();
};
