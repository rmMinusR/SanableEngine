#pragma once

#include "dllapi.h"

#include <cstdint>
#include <vector>
#include <optional>

typedef std::vector<std::optional<uint8_t>> DetectedVtables;

struct capture_utils
{
private:
	ENGINE_RTTI_API static DetectedVtables _captureVtablesInternal(std::initializer_list<void(*)()> thunks);

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
			static inline DetectedVtables captureVtables()
			{
				return _captureVtablesInternal(
					&thunk_newOnStack,
					&thunk_newOnHeap,
					&thunk_newInPlace
				);
			}
		};
	};
};
