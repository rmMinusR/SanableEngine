#pragma once

#include "dllapi.h"
#include "math/Vector2.inl"
#include "math/Rect.inl"

class Texture;

class Sprite
{
	Texture* tex;
	Rect<float> uvs;

	friend class Renderer;

public:
	ENGINEGRAPHICS_API Sprite(Texture* tex);
	ENGINEGRAPHICS_API ~Sprite();

	ENGINEGRAPHICS_API void setUVRect(Rect<float> uvs);
	ENGINEGRAPHICS_API void setPixelRect(Rect<int> pixelCoords);
};
