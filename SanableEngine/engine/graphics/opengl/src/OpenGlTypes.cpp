#include "OpenGlTypes.hpp"

#include <cassert>
#include <glm/glm.hpp>

#include "Texture.hpp"

TypeName getGlmType(GLenum openGlType)
{
	switch (openGlType)
	{
		#define _Type1D(openGlEnum, cxxBaseType, elementCount, glCallSuffix) \
			case openGlEnum: return TypeName::create<glm::vec<elementCount, cxxBaseType>>();

		#define _Type2D(openGlEnum, cxxBaseType, rowCount, colCount, glCallSuffix) \
			case openGlEnum: return TypeName::create<glm::mat<rowCount, colCount, cxxBaseType>>();

		#define _TypeSampler(openGlEnum, cxxBaseType, axisCount) \
			case openGlEnum: return TypeName::create<cxxBaseType>();

		FOREACH_OPENGL_TYPE(_Type1D, _Type2D, _TypeSampler)

		#undef _Type1D
		#undef _Type2D
		#undef _TypeSampler

	default:
		assert(false);
		return TypeName();
	}
}

GLenum getOpenGlType(TypeName type)
{
	#define _Type1D(openGlEnum, cxxBaseType, elementCount, glCallSuffix) \
		if(type == TypeName::create<glm::vec<elementCount, cxxBaseType>>()) return openGlEnum;

	#define _Type2D(openGlEnum, cxxBaseType, rowCount, colCount, glCallSuffix) \
		if(type == TypeName::create<glm::mat<rowCount, colCount, cxxBaseType>>()) return openGlEnum;

	#define _TypeSampler(openGlEnum, cxxBaseType, axisCount) /* undefined behavior: GTexture maps to multiple */

	FOREACH_OPENGL_TYPE(_Type1D, _Type2D, _TypeSampler)

	#undef _Type1D
	#undef _Type2D
	#undef _TypeSampler

	return 0;
}
