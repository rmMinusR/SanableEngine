#pragma once

#include "dllapi.h"
#include "math/Vector2.inl"
#include "math/Rect.inl"

class GTexture;

class Sprite
{
	const GTexture* tex;
	Rect<float> uvs;

	friend class Renderer;

public:
	ENGINEGRAPHICS_API Sprite(const GTexture* tex);
	ENGINEGRAPHICS_API ~Sprite();

	ENGINEGRAPHICS_API void setUVRect(Rect<float> uvs);
	ENGINEGRAPHICS_API void setPixelRect(Rect<int> pixelCoords);
};
