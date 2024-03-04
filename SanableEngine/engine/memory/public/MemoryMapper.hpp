#pragma once

/* 
 * Allows moving objects such that pointers can be mapped to new addresses
 * without data loss or serialization. Forms the backbone of V2 hot-reloads and GC.
 */

#include "dllapi.h"

#include <vector>

/// <summary>
/// An interface for moving that will allow pointers to be updated to new object addresses.
/// </summary>
class MemoryMapper
{
	/// <summary>
	/// INTERNAL. Represents a move that can be stored and replayed later.
	/// </summary>
	struct RemapOp
	{
		void* src;
		void* dst;
		size_t blockSize;
	
	private:
		friend class MemoryMapper;
		ENGINEMEM_API RemapOp() = default;
	};

	std::vector<RemapOp> opLog;

	static constexpr bool USE_INVALID_DATA_FILL = true;
	static constexpr unsigned char INVALID_DATA_FILL_VALUE = 219;

public:
	/// <summary>
	/// Like rawMove, but calls move ctor first. Undefined behaviour when hot reloading.
	/// </summary>
	template<typename T>
	void move(T* dst, T* src)
	{
		*dst = std::move(*src); //Do actual move op
		if (USE_INVALID_DATA_FILL) memset(src, INVALID_DATA_FILL_VALUE, sizeof(T)); //Fill old memory

		logMove(dst, src, sizeof(T));
	}

	/// <summary>
	/// Alters data, then logs move. Same syntax as memcpy.
	/// </summary>
	ENGINEMEM_API void rawMove(void* dst, void* src, size_t bytesToMove);

	/// <summary>
	/// Affects only pointer transformation, does not directly touch data. Used mainly for DLLs.
	/// </summary>
	ENGINEMEM_API void logMove(void* dst, void* src, size_t bytesToMove);

	/// <summary>
	/// Returns the address where the pointed object ended up.
	/// </summary>
	ENGINEMEM_API void* transformAddress(void* ptr, size_t ptrSize) const;

	template<typename T>
	inline T* transformAddress(T* ptr)
	{
		return (T*) transformAddress(ptr, sizeof(T)); //Defer to main impl
	}

	ENGINEMEM_API void clear();
};
