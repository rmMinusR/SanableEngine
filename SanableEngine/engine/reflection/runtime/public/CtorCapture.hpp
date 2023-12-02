#pragma once

#include "dllapi.h"
#include "CapstoneWrapper.hpp"

struct capture_utils
{
private:
	ENGINE_RTTI_API static DetectedConstants _captureVtablesInternal(size_t objSize, std::initializer_list<void(*)()> thunks);

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
			static inline DetectedConstants captureVtables()
			{
				return _captureVtablesInternal(sizeof(T), {
					&thunk_newOnStack,
					&thunk_newOnHeap,
					&thunk_newInPlace
				});
			}
		};
	};
};
