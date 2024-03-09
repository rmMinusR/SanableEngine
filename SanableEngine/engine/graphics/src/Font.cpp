#include "Font.hpp"

#include <cassert>

#include "Renderer.hpp"
#include "Texture.hpp"

std::weak_ptr<FreetypeHandle> FreetypeHandle::instance;

std::shared_ptr<FreetypeHandle> FreetypeHandle::getInstance()
{
	std::shared_ptr<FreetypeHandle> out;
	if (instance.expired())
	{
		out = std::make_shared<FreetypeHandle>();
		instance = out;
	}
	else out = instance.lock();
	return out;
}

FreetypeHandle::FreetypeHandle()
{
	FT_Error err = FT_Init_FreeType(&ft);
	if (err)
	{
		printf("ERROR: Could not init FreeType Library: code %i\n", err);
		ft = nullptr;
		assert(false);
	}
}

FreetypeHandle::~FreetypeHandle()
{
	if (ft)
	{
		FT_Done_FreeType(ft);
		ft = nullptr;
	}
}

FreetypeHandle::operator FT_Library() const
{
	return ft;
}

FreetypeHandle::operator bool() const
{
	return ft != nullptr;
}

bool Font::activateGlyph(wchar_t data, int flags) const
{
	unsigned long charIndex = FT_Get_Char_Index(font, data);
	FT_Error err = FT_Load_Glyph(font, charIndex, flags);
	return !err;
}

RenderedGlyph::RenderedGlyph() :
	texture(nullptr),
	bearingX(0),
	bearingY(0),
	advance(0)
{
}

RenderedGlyph::~RenderedGlyph()
{
}

RenderedGlyph::operator bool() const
{
	return texture != nullptr;
}

RenderedGlyph::RenderedGlyph(RenderedGlyph&& mov) :
	RenderedGlyph()
{
	*this = std::move(mov);
}

RenderedGlyph& RenderedGlyph::operator=(RenderedGlyph&& mov)
{
	if (texture) delete texture;
	memcpy(this, &mov, sizeof(RenderedGlyph));
	memset(&mov, 0, sizeof(RenderedGlyph));
	return *this;
}

RenderedGlyph const* Font::getGlyph(wchar_t data, Renderer* renderer) const
{
	//Attempt to lookup from cache
	auto it = cache.find(data);
	if (it != cache.end()) return &it->second;

	//Otherwise, render

	//Load desired character glyph
	if (!activateGlyph(data, FT_LOAD_RENDER)) return nullptr;

	RenderedGlyph glyph;
	glyph.texture = renderer->renderFontGlyph(*this);
	glyph.bearingX = font->glyph->bitmap_left;
	glyph.bearingY = font->glyph->bitmap_top;
	glyph.advance  = font->glyph->advance.x;
	cache[data] = std::move(glyph);
	
	return &cache[data];
}

RenderedGlyph const* Font::getFallbackGlyph(Renderer* renderer) const
{
	RenderedGlyph const* out = nullptr;
	out = getGlyph(0xFFFD, renderer); if (out) return out; //Question diamond
	out = getGlyph(0x25A1, renderer); if (out) return out; //White square
	out = getGlyph(L'?'  , renderer); if (out) return out; //Basic ASCII question mark
	assert(false && "No fallback glyph available!");
	return nullptr;
}

Font::Font(const std::filesystem::path& path, int size) :
	Font(path, size, 0)
{
}

Font::Font(const std::filesystem::path& path, int size, int index)
{
	libHandle = FreetypeHandle::getInstance();

	FT_Error err = FT_New_Face(*libHandle, path.string().c_str(), index, &font); //TODO: wstring support
	if (err)
	{
		wprintf(L"ERROR: Failed to load font '%ls': code %i", path.wstring().c_str(), err);
		font = nullptr;
		assert(false);
	}

	if (font)
	{
		FT_Set_Pixel_Sizes(font, 0, size);
		FT_Select_Charmap(font, FT_ENCODING_UNICODE);
		this->size = size;
	}
}

Font::~Font()
{
	for (auto& i : cache) delete i.second.texture;
	cache.clear();

	if (font)
	{
		FT_Done_Face(font);
		font = nullptr;
	}
}
