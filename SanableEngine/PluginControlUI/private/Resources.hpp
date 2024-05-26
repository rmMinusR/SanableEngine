#pragma once

class Material;
class Font;
class GTexture;
class UISprite3x3;

#define FOREACH_RESOURCE() \
	_X(Font*, headerFont) \
	_X(Font*, labelFont) \
	_X(Material*, textMat) \
	_X(Material*, imageMat) \
	_X(GTexture*   , buttonNormalTexture) \
	_X(UISprite3x3*, buttonNormalSprite) \
	_X(GTexture*   , buttonPressedTexture) \
	_X(UISprite3x3*, buttonPressedSprite) \
	_X(GTexture*   , buttonDisabledTexture) \
	_X(UISprite3x3*, buttonDisabledSprite) \
	_X(GTexture*   , rttiFieldTexture) \
	_X(UISprite3x3*, rttiFieldSprite) \
	_X(GTexture*   , rttiParentTexture) \
	_X(UISprite3x3*, rttiParentSprite)


namespace Resources
{
	#define _X(type, name) extern type name;
	FOREACH_RESOURCE()
	#undef _X
}
