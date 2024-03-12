#pragma once

class Material;
class Font;

#define FOREACH_RESOURCE() \
	_X(Font*, headerFont) \
	_X(Font*, labelFont) \
	_X(Material*, textMat) \
	_X(Material*, imageMat)


namespace Resources
{
	#define _X(type, name) extern type name;
	FOREACH_RESOURCE()
	#undef _X
}
