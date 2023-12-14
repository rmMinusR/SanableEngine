#pragma once

#include "dllapi.h"
#include "DetectedConstants.hpp"

struct capture_utils
{
private:
	ENGINE_RTTI_API static DetectedConstants _captureVtablesInternal(size_t objSize, void(*thunk)(), const std::vector<void(*)()>& allocators, const std::vector<void(*)()>& nofill);

public:
	capture_utils() = delete;

	//Helper for SemanticVM ThisPtr detection
	constexpr static uint32_t dummyAllocatedValue = 0xDEADBEEF;
	template<int> static void* dummyAllocator() { return (void*)dummyAllocatedValue; }

	template<typename T>
	struct type
	{
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
	};
};
