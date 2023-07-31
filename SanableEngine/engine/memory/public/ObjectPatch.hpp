#pragma once

#include "TypeInfo.hpp"

struct ObjectPatch
{
	TypeInfo oldData;
	TypeInfo newData;

	ENGINEMEM_API void apply(void* target, MemoryMapper* remapLog = nullptr) const;
	ENGINEMEM_API bool isValid() const;
	ENGINEMEM_API TypeName getTypeName() const;

	ENGINEMEM_API void debugLog() const;

	ENGINEMEM_API static ObjectPatch create(TypeInfo oldData, TypeInfo newData);
};
