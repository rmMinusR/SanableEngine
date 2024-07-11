#pragma once

#include "TypeInfo.hpp"

#include "dllapi.h"

class ObjectRelocator;

struct ObjectPatch
{
	TypeInfo oldData;
	TypeInfo newData;

	ENGINEMEM_API void apply(void* target, ObjectRelocator* remapLog = nullptr) const;
	ENGINEMEM_API bool isValid() const;
	ENGINEMEM_API TypeName getTypeName() const;

	ENGINEMEM_API void debugLog() const;

	ENGINEMEM_API static ObjectPatch create(TypeInfo oldData, TypeInfo newData);
};
