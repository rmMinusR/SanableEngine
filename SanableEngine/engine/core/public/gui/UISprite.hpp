#pragma once 

#include "Sprite.hpp"
#include "dllapi.h"


class UISprite
{
public:
	ENGINEGUI_API virtual ~UISprite();
	virtual Sprite get(Vector2<int> index) const = 0;
};


class UISprite3x3 : public UISprite
{
	Texture* tex;
	Vector2f uvs[4];

public:
	ENGINEGUI_API UISprite3x3(Texture* tex);
	ENGINEGUI_API virtual ~UISprite3x3();

	ENGINEGUI_API void set(Vector2<int> index, Vector2f uv);
	ENGINEGUI_API virtual Sprite get(Vector2<int> index) const override;
};

class UISpriteSparse : public UISprite
{
	Texture* tex;
	Rect<float> uvs[9];

public:
	ENGINEGUI_API UISpriteSparse(Texture* tex);
	ENGINEGUI_API virtual ~UISpriteSparse();

	ENGINEGUI_API void set(Vector2<int> index, Rect<float> uv);
	ENGINEGUI_API virtual Sprite get(Vector2<int> index) const override;
};
