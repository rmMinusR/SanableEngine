#pragma once

#include "TypeInfo.hpp"

#include <cassert>

class ModuleTypeRegistry;

class TypeBuilder
{
	TypeInfo type;

	ENGINE_RTTI_API TypeBuilder();

	ENGINE_RTTI_API void addParent_internal(const TypeName& parent, size_t offset); //Order independent
public:
	template<typename TObj, typename... TCtorArgs>
	static TypeBuilder create(TCtorArgs... ctorArgs)
	{
		TypeBuilder out;
		out.type = TypeInfo::createDummy<TObj>();
		return out;
	}

	template<typename TObj, typename... TCtorArgs>
	void captureCDO(TCtorArgs... ctorArgs)
	{
		assert(TypeName::create<TObj>() == type.name);

		//Prepare memory
		char mem1[sizeof(TObj)];
		char mem2[sizeof(TObj)];
		char mem3[sizeof(TObj)];
		memset(mem1, 0x00, sizeof(mem1));
		memset(mem2, 0x88, sizeof(mem2));
		memset(mem3, 0xFF, sizeof(mem3));

		//Create CDOs
		TObj* cdo1 = new (mem1) TObj(ctorArgs...);
		TObj* cdo2 = new (mem2) TObj(ctorArgs...);
		TObj* cdo3 = new (mem3) TObj(ctorArgs...);

		//Magic!
		//captureCDO_internal({ cdo1, cdo2, cdo3 });

		//Clean up
		cdo1->~TObj();
		cdo2->~TObj();
		cdo3->~TObj();
	}

	//Order independent
	//DO NOT USE for virtual inheritance
	template<typename TObj, typename TParent>
	inline void addParent()
	{
		//No funny business!
		static_assert(std::is_base_of<TParent, TObj>::value);
		assert(TypeName::create<TObj>() == type.name);
		
		constexpr TObj* root = nullptr; //Arbitrary
		constexpr TParent* parent = root;
		size_t offset = size_t( ((char*)parent) - ((char*)root) );
		addParent_internal(TypeName::create<TParent>(), offset);
	}

	ENGINE_RTTI_API void addField(const TypeName& type, const std::string& name, size_t offset); //Order independent
	ENGINE_RTTI_API void registerType(ModuleTypeRegistry* registry); //Non-const: Make it hard to register if we aren't the mutable original. For the same reason, not on TypeInfo.
};
