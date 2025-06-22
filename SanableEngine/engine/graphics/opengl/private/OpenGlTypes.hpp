#pragma once

#include <GL/glew.h>
#include <TypeName.hpp>

TypeName getGlmType(GLenum openGlType);
GLenum getOpenGlType(TypeName type); // Returns 0 if no clear mapping


// _Type1D(openGlEnum, cxxBaseType, elementCount, glCallSuffix)
// _Type2D(openGlEnum, cxxBaseType, rowCount, colCount, glCallSuffix)
// _TypeSampler(openGlEnum, cxxBaseType, axisCount)

#define _VectorTypeFamily(_Type1D, openGlEnumBase, cxxBaseType, glCallSuffix) \
	_Type1D(openGlEnumBase       , cxxBaseType, 1, 1##glCallSuffix) \
	_Type1D(openGlEnumBase##_VEC2, cxxBaseType, 2, 2##glCallSuffix) \
	_Type1D(openGlEnumBase##_VEC3, cxxBaseType, 3, 3##glCallSuffix) \
	_Type1D(openGlEnumBase##_VEC4, cxxBaseType, 4, 4##glCallSuffix)

#define _MatrixTypeFamily(_Type2D, openGlEnumBase, cxxBaseType, glCallSuffix) \
	_Type2D(openGlEnumBase##2  , cxxBaseType, 2, 2, 2  ) \
	_Type2D(openGlEnumBase##2x3, cxxBaseType, 2, 3, 2x3) \
	_Type2D(openGlEnumBase##2x4, cxxBaseType, 2, 4, 2x4) \
	_Type2D(openGlEnumBase##3x2, cxxBaseType, 3, 2, 3x2) \
	_Type2D(openGlEnumBase##3  , cxxBaseType, 3, 3, 3  ) \
	_Type2D(openGlEnumBase##3x4, cxxBaseType, 3, 4, 3x4) \
	_Type2D(openGlEnumBase##4x2, cxxBaseType, 4, 2, 4x2) \
	_Type2D(openGlEnumBase##4x3, cxxBaseType, 4, 3, 4x3) \
	_Type2D(openGlEnumBase##4  , cxxBaseType, 4, 4, 4  )

#define FOREACH_OPENGL_TYPE(_Type1D, _Type2D, _TypeSampler) \
	_VectorTypeFamily(_Type1D, GL_FLOAT       , float   , f ) \
	_VectorTypeFamily(_Type1D, GL_DOUBLE      , double  , d ) \
	_VectorTypeFamily(_Type1D, GL_INT         , int32_t , i ) \
	_VectorTypeFamily(_Type1D, GL_UNSIGNED_INT, uint32_t, ui) \
	_VectorTypeFamily(_Type1D, GL_BOOL        , bool    , b ) \
	_MatrixTypeFamily(_Type2D, GL_FLOAT_MAT   , float   , fv) \
	_MatrixTypeFamily(_Type2D, GL_DOUBLE_MAT  , double  , dv) \
	_TypeSampler(GL_SAMPLER_2D, GTexture, 2)
