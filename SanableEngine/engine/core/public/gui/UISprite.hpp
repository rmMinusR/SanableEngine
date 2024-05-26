#pragma once 

#include "Sprite.hpp"
#include "dllapi.h"
#include "math/Vector3.inl"

class Renderer;


class UISprite
{
public:
	ENGINEGUI_API virtual ~UISprite();
	virtual Sprite get(Vector2<int> index) const = 0;
	virtual void renderImmediate(Renderer* renderer, Vector3f pos, float w, float h) const = 0;
};


class UISprite3x3 : public UISprite
{
	GTexture* tex;
	Vector2f uvs[4];

public:
	ENGINEGUI_API UISprite3x3(GTexture* tex);
	ENGINEGUI_API virtual ~UISprite3x3();

	ENGINEGUI_API void set(Vector2<int> index, Vector2f uv);
	ENGINEGUI_API void setPixel(Vector2<int> index, Vector2f uv);
	ENGINEGUI_API virtual Sprite get(Vector2<int> index) const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer, Vector3f pos, float w, float h) const override;
};

class UISpriteSparse : public UISprite
{
	GTexture* tex;
	Rect<float> uvs[9];

public:
	ENGINEGUI_API UISpriteSparse(GTexture* tex);
	ENGINEGUI_API virtual ~UISpriteSparse();

	ENGINEGUI_API void set(Vector2<int> index, Rect<float> uv);
	ENGINEGUI_API virtual Sprite get(Vector2<int> index) const override;
	ENGINEGUI_API virtual void renderImmediate(Renderer* renderer, Vector3f pos, float w, float h) const override;
};
