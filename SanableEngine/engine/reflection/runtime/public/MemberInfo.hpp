#pragma once

#include "dllapi.h"

#include <cstddef>

struct MemberInfo
{
	size_t size;
	ptrdiff_t offset;

protected:
	ENGINE_RTTI_API void* getAddr(void* objInstance) const;
};

enum class MemberVisibility
{
	Private   = 1<<0,
	Protected = 1<<1,
	Public    = 1<<2,

	All = ~0
};
