#pragma once

#include <type_traits>

#include "TypeName.hpp"
#include "GlobalTypeRegistry.hpp"

struct TypeInfo;

namespace stix
{
	//An owning reference to an object, similar to std::unique_ptr
	//Causes 1-2 heap allocations
	template<typename TConstraint = void>
	class OwningRef;


	class _OwningRefImpl
	{
	public:
		STIX_API ~_OwningRefImpl();

		STIX_API _OwningRefImpl(_OwningRefImpl&& mov);
		STIX_API _OwningRefImpl& operator=(_OwningRefImpl&& mov);

		STIX_API operator bool() const;
		STIX_API bool valid() const;

		STIX_API const TypeInfo* type() const;

	protected:
		STIX_API void put(void* obj, void(*deleter)(void*), const TypeInfo* typeInfo, bool ownsTypeInfo);
		STIX_API void clear();
		STIX_API void* try_get_as(const TypeName& ty);

		STIX_API _OwningRefImpl();

		// Disable accidental copying
		_OwningRefImpl(const _OwningRefImpl& cpy) = delete;
		_OwningRefImpl& operator=(const _OwningRefImpl& cpy) = delete;

		void* obj = nullptr;
		void(*deleter)(void*) = nullptr;
		const TypeInfo* objType = nullptr;
		bool ownsTypeInfo = false;
	};

	template<typename TConstraint>
	class OwningRef : private _OwningRefImpl
	{
		template<typename T>
		constexpr static bool constraint_satisfied = std::is_same_v<T, TConstraint> || std::is_base_of_v<TConstraint, T> || std::is_same_v<TConstraint, void>;

	public:
		using _OwningRefImpl::operator bool;
		using _OwningRefImpl::valid;
		using _OwningRefImpl::type;
		using _OwningRefImpl::clear;

		template<typename T>
		void put(T* obj, void(*deleter)(void*) = nullptr)
		{
			static_assert(constraint_satisfied<T>, "Given type breaks type constraint");

			bool ownsTypeInfo = false;
			const TypeInfo* ty = TypeName::create<T>();
			if (!ty)
			{
				// Warning: RTTI missing
				ty = new TypeInfo(TypeInfo::createDummy<T>());
				ownsTypeInfo = true;
			}

			if constexpr (std::is_polymorphic_v<T>)
			{
				// Ensure user is passing what they claim
				assert(GlobalTypeRegistry::snipeType(obj, typeInfo->layout.size, typeInfo)->name == ty->name);
			}

			_OwningRefImpl::put(obj, deleter ? deleter : &_deleter_thunk<T>, ty, ownsTypeInfo);
		}

		template<typename T>
		OwningRef& operator=(T&& obj)
		{
			put(new T(std::move(obj)));
			return *this;
		}

		template<typename T>
		T* as()
		{
			static_assert(constraint_satisfied<T>, "Cannot cast to given type");
			void* casted = _OwningRefImpl::try_get_as(TypeName::create<T>());
			return reinterpret_cast<T*>(casted);
		}

	private:
		template<typename T>
		static void _deleter_thunk(void* obj)
		{
			delete reinterpret_cast<T*>(obj);
		}
	};
}
