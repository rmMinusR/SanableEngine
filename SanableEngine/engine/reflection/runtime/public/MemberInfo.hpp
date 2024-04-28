#pragma once

#include <cstddef>

#include "dllapi.h"
#include "TypeName.hpp"

struct MemberInfo
{
	size_t size;
	ptrdiff_t offset;
	TypeName owner;

protected:
	MemberInfo(size_t size, ptrdiff_t offset, const TypeName& owner);
	ENGINE_RTTI_API void* getAddr(void* objInstance) const;
};

enum class MemberVisibility
{
	Private   = 1<<0,
	Protected = 1<<1,
	Public    = 1<<2,

	All = ~0
};
