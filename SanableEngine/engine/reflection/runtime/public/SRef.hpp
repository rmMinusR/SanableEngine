#pragma once

#include <type_traits>

#include "TypeName.hpp"

struct TypeInfo;

namespace stix
{

	class MemberFunction;
	class StaticFunction;

	//Pass objects by reference while also erasing their type. Similar to Object in Java/C#/Python.
	//Does not own the given object; should never be saved in an object.
	class SRef
	{
		void* data;
		TypeName type;

		friend class ::stix::MemberFunction;
		friend class ::stix::StaticFunction;

		STIX_API void* get_internal(const TypeName& asType) const;
		STIX_API void* get_unchecked() const;
		STIX_API SRef(void* data, const TypeName& type);
	public:
		STIX_API SRef();
		STIX_API ~SRef();

		template<typename T>
		static SRef make(T* obj)
		{
			static_assert(!std::is_reference_v<T>);
			return SRef(obj, TypeName::create<T>()); //TODO attempt to snipe RTTI and checK, just to be sure? Casting should handle most of it though.
		}

		STIX_API TypeName getType() const;
		template<typename T>
		std::remove_reference_t<T>& get() const { return *(std::remove_reference_t<T>*)get_internal(TypeName::tryCreate<T>()); }

		STIX_API operator bool() const;
		STIX_API bool has_value() const;

		//Allow copy and move
		SRef(const SRef& cpy) = default;
		SRef& operator=(const SRef& cpy) = default;
		SRef(SRef&& mov) = default;
		SRef& operator=(SRef&& mov) = default;
	};

	namespace detail
	{
		static inline decltype(auto) _getRepresentedType(const SRef& v) { return v.getType(); }
	}
}
