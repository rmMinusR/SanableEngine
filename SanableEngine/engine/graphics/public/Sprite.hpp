#pragma once

#include "Vector2.inl"
#include "dllapi.h"
#include "Rect.hpp"

class GTexture;

class Sprite
{
	GTexture* tex;
	Rect<float> uvs;

	friend class Renderer;

public:
	ENGINEGRAPHICS_API Sprite(GTexture* tex);
	ENGINEGRAPHICS_API ~Sprite();

	ENGINEGRAPHICS_API void setUVRect(Rect<float> uvs);
	ENGINEGRAPHICS_API void setPixelRect(Rect<int> pixelCoords);
};
