#include "Sprite.hpp"

#include "Texture.hpp"

Sprite::Sprite(GTexture* tex) :
	tex(tex)
{
}

Sprite::~Sprite()
{
}

void Sprite::setUVRect(Rect<float> uvs)
{
	this->uvs = uvs;
}

void Sprite::setPixelRect(Rect<int> pixelCoords)
{
	Vector2f texSize = tex->getSize().convert<float>();
	setUVRect(Rect<float>
	{
		pixelCoords.topLeft.convert<float>() / texSize,
		pixelCoords.size   .convert<float>() / texSize
	});
}
