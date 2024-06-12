#include "gui/UISprite.hpp"

#include "Texture.hpp"
#include "Renderer.hpp"

UISprite::~UISprite()
{
}

UISprite3x3::UISprite3x3(GTexture* tex) :
	tex(tex)
{
	uvs[0] = Vector2f(0, 0);
	uvs[1] = Vector2f(1/3.0f, 1/3.0f);
	uvs[2] = Vector2f(2/3.0f, 2/3.0f);
	uvs[3] = Vector2f(1, 1);
}

UISprite3x3::~UISprite3x3()
{
}

void UISprite3x3::set(Vector2<int> index, Vector2f uv)
{
	uvs[index.x].x = uv.x;
	uvs[index.y].y = uv.y;
}

void UISprite3x3::setPixel(Vector2<int> index, Vector2f uv)
{
	set(index, uv/Vector2f(tex->getWidth(), tex->getHeight()));
}

Sprite UISprite3x3::get(Vector2<int> index) const
{
	Sprite out(tex);
	out.setUVRect(Rect<float>::fromMinMax(
		{ uvs[index.x+0].x, uvs[index.y+0].y },
		{ uvs[index.x+1].x, uvs[index.y+1].y }
	));
	return out;
}

void UISprite3x3::renderImmediate(Renderer* renderer, const Material* mat, Vector3f pos, Vector2f size, SDL_Color tintColor) const
{
	//Calc corners
	Vector2f locs[4];
	locs[0] = pos.xy();
	locs[3] = locs[0] + size;

	//Calc inners using corner sizes
	Vector2f texSize(tex->getWidth(), tex->getHeight());
	locs[1] = locs[0] + (uvs[1]-uvs[0])*texSize;
	locs[2] = locs[3] - (uvs[3]-uvs[2])*texSize;

	//Fix elements smaller than minimum size
	if (locs[1].x > locs[2].x) locs[1].x = locs[2].x = (locs[1].x+locs[2].x)/2;
	if (locs[1].y > locs[2].y) locs[1].y = locs[2].y = (locs[1].y+locs[2].y)/2;

	for (int ix = 0; ix < 3; ++ix)
	{
		for (int iy = 0; iy < 3; ++iy)
		{
			Sprite s = get(Vector2<int>(ix, iy));
			renderer->drawSprite(
				&s,
				mat,
				Vector3f(locs[ix].x, locs[iy].y, pos.z),
				locs[ix+1].x-locs[ix].x,
				locs[iy+1].y-locs[iy].y,
				tintColor
			);
		}
	}
}

UISpriteSparse::UISpriteSparse(GTexture* tex)
{
}

UISpriteSparse::~UISpriteSparse()
{
}

void UISpriteSparse::set(Vector2<int> index, Rect<float> uv)
{
	assert(0 <= index.x && index.x < 3);
	assert(0 <= index.y && index.y < 3);

	uvs[index.x + index.y*3] = uv;
}

Sprite UISpriteSparse::get(Vector2<int> index) const
{
	Sprite out(tex);
	out.setUVRect(uvs[index.x + index.y*3]);
	return out;
}

void UISpriteSparse::renderImmediate(Renderer* renderer, const Material* mat, Vector3f pos, Vector2f size, SDL_Color tintColor) const
{
	assert(false && "TODO implement");
}
