#pragma once 

#include "Sprite.hpp"
#include "dllapi.h"

class UISprite3x3
{
	GTexture* tex;
	Vector2f uvs[4];

public:
	ENGINEGUI_API UISprite3x3(GTexture* tex);
	ENGINEGUI_API ~UISprite3x3();

	ENGINEGUI_API void set(Vector2<int> index, Vector2f uv);
	ENGINEGUI_API Sprite get(Vector2<int> index) const;
};
