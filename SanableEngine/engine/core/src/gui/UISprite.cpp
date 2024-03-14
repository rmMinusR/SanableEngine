#include "gui/UISprite.hpp"

UISprite3x3::UISprite3x3(GTexture* tex) :
	tex(tex)
{
	uvs[0] = Vector2f(0, 0);
	uvs[1] = Vector2f(0.3f, 0.3f);
	uvs[2] = Vector2f(0.6f, 0.6f);
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

Sprite UISprite3x3::get(Vector2<int> index) const
{
	Sprite out(tex);
	out.setUVRect(Rect<float>::fromMinMax(
		uvs[index.x+0],
		uvs[index.x+1]
	));
	return out;
}
