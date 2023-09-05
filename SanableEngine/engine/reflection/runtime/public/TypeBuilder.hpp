#pragma once

#include "TypeInfo.hpp"
#include "ParentInfoBuilder.hpp"

#include <cassert>

class ModuleTypeRegistry;

class TypeBuilder
{
private:
	TypeInfo type;

	std::vector<ParentInfoBuilder> pendingParents;
	
	ENGINE_RTTI_API TypeBuilder();

	ENGINE_RTTI_API void addParent_internal(const TypeName& parent, size_t size, const std::function<void*(void*)>& upcastFn, MemberVisibility visibility, ParentInfo::Virtualness virtualness); //Order independent. UpcastFn must be valid when captureCDO or registerType are called.
	ENGINE_RTTI_API void addField_internal(const TypeName& declaredType, const std::string& name, size_t size, size_t offset); //Order independent
	ENGINE_RTTI_API void captureCDO_internal(const std::vector<void*>& instances);

public:
	template<typename TObj>
	static TypeBuilder create()
	{
		TypeBuilder out;
		out.type = TypeInfo::createDummy<TObj>();
		return out;
	}

	ENGINE_RTTI_API void registerType(ModuleTypeRegistry* registry);

	//Order independent
	//Ok to use for virtual inheritance
	template<typename TObj, typename TParent>
	inline void addParent(MemberVisibility visibility, ParentInfo::Virtualness virtualness)
	{
		//No funny business!
		static_assert(std::is_base_of<TParent, TObj>::value);
		assert(TypeName::create<TObj>() == type.name);

		addParent_internal(
			TypeName::create<TParent>(),
			sizeof(TParent),
			[](void* obj)
			{
				return static_cast<TParent*>( reinterpret_cast<TObj*>(obj) );
			},
			visibility,
			virtualness
		);
	}

	//Order independent
	template<typename TField>
	inline void addField(const std::string& name, size_t offset)
	{
		addField_internal(TypeName::create<TField>(), name, sizeof(TField), offset);
	}

	//Only call once all fields and parents are registered
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
		captureCDO_internal({ cdo1, cdo2, cdo3 });

		//Clean up
		cdo1->~TObj();
		cdo2->~TObj();
		cdo3->~TObj();
	}
};
