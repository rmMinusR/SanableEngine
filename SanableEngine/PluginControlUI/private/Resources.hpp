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
	_X(GTexture*, buttonBackgroundTexture) \
	_X(UISprite3x3*, buttonBackgroundSprite)


namespace Resources
{
	#define _X(type, name) extern type name;
	FOREACH_RESOURCE()
	#undef _X
}
