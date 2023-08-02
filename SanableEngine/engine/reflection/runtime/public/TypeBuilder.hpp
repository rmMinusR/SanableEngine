#pragma once

#include "TypeInfo.hpp"

class ModuleTypeRegistry;

class TypeBuilder
{
	TypeInfo type;

	ENGINE_RTTI_API TypeBuilder();
public:
	template<typename TObj, typename... TCtorArgs>
	static TypeBuilder fromCDO(bool definesNewVirtuals, TCtorArgs... ctorArgs)
	{
		char tmp[sizeof(TObj)];
		TObj* cdo = new(tmp) TObj(ctorArgs...);

		TypeBuilder out;
		out.type = TypeInfo::createDummy<TObj>();
		if (definesNewVirtuals) out.type.vtable = get_vtable_ptr(&cdo);

		cdo->~TObj();
		return out;
	}

	ENGINE_RTTI_API void addField(const TypeName& type, const std::string& localName); //Order matters
	ENGINE_RTTI_API void addParent(const TypeName& parent); //Order matters

	ENGINE_RTTI_API void registerType(ModuleTypeRegistry* registry); //Non-const: Make it hard to register if we aren't the mutable original. For the same reason, not on TypeInfo.
};
