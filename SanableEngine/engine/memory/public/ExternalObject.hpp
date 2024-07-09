#pragma once

#include <cstdint>

#include "TypeName.hpp"

enum class ExternalObjectOptions : uint8_t
{
	AllowDelete         = 1<<0, //Can this object be deleted?
	AllowMoveRoot       = 1<<1, //Can this whole object be moved?
	AllowMoveFields     = 1<<2, //Can this object's fields be moved?
	AllowFieldRecursion = 1<<3, //Can this object's fields be recursed into?
	AllowPointerFixing  = 1<<4, //Can moves cause the pointers in this object to be updated? (and possibly children via AllowFieldRecursion)


	None = 0,
	All = AllowDelete | AllowMoveRoot | AllowMoveFields | AllowFieldRecursion | AllowPointerFixing,

	DefaultInternal = All,
	DefaultExternal = AllowFieldRecursion | AllowPointerFixing
};
