#pragma once

#include "TypeInfo.hpp"
#include "ParentInfoBuilder.hpp"
#include "FieldInfoBuilder.hpp"

#include <cassert>
#include <functional>

class ModuleTypeRegistry;

class TypeBuilder
{
private:
	TypeInfo type;

	std::vector<ParentInfoBuilder> pendingParents;
	std::vector<FieldInfoBuilder> pendingFields;
	
	ENGINE_RTTI_API TypeBuilder();

	ENGINE_RTTI_API void addParent_internal(const TypeName& parent, size_t size, const std::function<void*(void*)>& upcastFn, MemberVisibility visibility, ParentInfo::Virtualness virtualness); //Order independent. UpcastFn must be valid when captureCDO or registerType are called.
	ENGINE_RTTI_API void addField_internal(const TypeName& declaredType, const std::string& name, size_t size, std::function<ptrdiff_t(const void*)> accessor, MemberVisibility visibility); //Order independent. Accessor must be valid after captureCDO is called.
	ENGINE_RTTI_API void captureClassImage_internal(std::function<void(void*)> ctor, std::function<void(void*)> dtor);

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
	inline void addField(const std::string& name, std::function<ptrdiff_t(const void*)> accessor)
	{
		addField_internal(TypeName::create<TField>(), name, sizeof(TField), accessor, MemberVisibility::Public); // TODO extract visibility in RTTI generation step
	}

	//Only call once all fields and parents are registered
	template<typename TObj, typename... TCtorArgs>
	void captureClassImage(TCtorArgs... ctorArgs)
	{
		assert(TypeName::create<TObj>() == type.name);

		//Magic!
		captureClassImage_internal(
			[&](void* obj) { new (obj) TObj(ctorArgs...); },
			[](void* obj) { static_cast<TObj*>(obj)->~TObj(); }
		);
	}
};
