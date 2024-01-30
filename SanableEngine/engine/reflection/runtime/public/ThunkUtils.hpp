#pragma once

#include "dllapi.h"
#include "DetectedConstants.hpp"

typedef void (*dtor_t)(void*); //CANNOT be a std::function or lambda because destroying the dtor_t instance would attempt to delete memory from a possibly-unloaded plugin


ENGINE_RTTI_API ptrdiff_t _captureCastOffset(const DetectedConstants& image, void*(*castThunk)(void*)); //TODO implement
ENGINE_RTTI_API DetectedConstants _captureVtablesInternal(size_t objSize, void(*thunk)(), const std::vector<void(*)()>& allocators, const std::vector<void(*)()>& nofill);
//Helper for SemanticVM ThisPtr detection
template<int> void* dummyAllocator() { return nullptr; }


template<typename T>
struct thunk_utils
{
	thunk_utils() = delete;

	template<typename... Args>
	struct ctor
	{
	private:
		static void thunk_newInPlace(Args... args) { new(dummyAllocator<0>()) T(args...); }

	public:
		static inline DetectedConstants captureVtables()
		{
			return _captureVtablesInternal(
				sizeof(T),
				(void(*)()) &thunk_newInPlace,
				{ (void(*)()) &dummyAllocator<0> },
				{ (void(*)()) &memset } //Some compilers will pre-zero, especially in debug mode. Don't catch that. NOTE: &memset will be unique per-module.
			);
		}
	};
		
	#pragma region Destructor

	template<bool has_destructor>
	struct _dtor { _dtor() = delete; };

	template<>
	struct _dtor<true>
	{
		//C++ forbids getting the address of a dtor, but we can still wrap it
		inline static void call_dtor(void* obj) { static_cast<const T*>(obj)->~T(); }

		constexpr static dtor_t dtor = &call_dtor;
	};

	template<>
	struct _dtor<false>
	{
		//Can't call a dtor that doesn't exist
		constexpr static dtor_t dtor = nullptr;
	};

	constexpr static dtor_t dtor = ::thunk_utils<T>::_dtor<std::is_destructible<T>::value>::dtor;

	#pragma endregion
};
