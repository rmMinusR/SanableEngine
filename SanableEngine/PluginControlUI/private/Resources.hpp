#pragma once

class Material;
class Font;
class GTexture;

#define FOREACH_RESOURCE() \
	_X(Font*, headerFont) \
	_X(Font*, labelFont) \
	_X(Material*, textMat) \
	_X(Material*, imageMat) \
	_X(GTexture*, buttonBackground)


namespace Resources
{
	#define _X(type, name) extern type name;
	FOREACH_RESOURCE()
	#undef _X
}
