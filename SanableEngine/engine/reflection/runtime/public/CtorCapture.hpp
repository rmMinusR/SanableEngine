#pragma once

#include "dllapi.h"
#include "DetectedConstants.hpp"

struct capture_utils
{
private:
	ENGINE_RTTI_API static DetectedConstants _captureVtablesInternal(size_t objSize, std::initializer_list<void(*)()> thunks, std::initializer_list<void(*)()> blacklist, bool* wasBlacklistTripped_out_optional);

public:
	capture_utils() = delete;

	template<typename T>
	struct type
	{
		template<typename... Args>
		struct ctor
		{
		private:
			static void thunk_newOnStack(        Args... args) {          T v(args...); }
			static void thunk_newOnHeap (        Args... args) { new      T  (args...); }
			static void thunk_newInPlace(T* ptr, Args... args) { new(ptr) T  (args...); }

		public:
			static inline DetectedConstants captureVtables(bool* wasPreZeroed_out_optional = nullptr)
			{
				return _captureVtablesInternal(
					sizeof(T),
					{
						(void(*)()) &thunk_newOnStack,
						(void(*)()) &thunk_newOnHeap,
						(void(*)()) &thunk_newInPlace
					},
					{ (void(*)()) &memset }, //Some compilers will pre-zero, especially in debug mode. Don't catch that.
					wasPreZeroed_out_optional
				);
			}
		};
	};
};
