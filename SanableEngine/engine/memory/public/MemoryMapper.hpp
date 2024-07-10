#pragma once

/* 
 * Allows moving objects such that pointers can be mapped to new addresses
 * without data loss or serialization. Forms the backbone of V2 hot-reloads and GC.
 */

#include "dllapi.h"

#include <vector>
#include <set>

struct TypeName;
struct TypeInfo;

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
	/// Returns the address where the pointed object ended up. Doesn't do any recursion into the object in question.
	/// </summary>
	ENGINEMEM_API void* transformAddress(void* ptr, size_t ptrSize) const;

	template<typename T>
	inline T* transformAddress(T* ptr)
	{
		return (T*) transformAddress(ptr, sizeof(T)); //Defer to main impl
	}

	// Only takes objects, doesn't take pointers. Also doesn't check recursePointers on self.
	ENGINEMEM_API void transformComposite(void* object, const TypeInfo* type, bool recurseFields, std::set<void*>* recursePointers) const;

	/// <summary>
	/// Transform all affected addresses in an object. Also takes addresses.
	/// </summary>
	/// <param name="object">Object to scan and update addresses</param>
	/// <param name="type">Type of object to scan</param>
	/// <param name="recurseFields">Should we recurse into fields?</param>
	/// <param name="recursePointers">If non-null, we should recurse into pointers (and record it here so we don't hit it twice)</param>
	ENGINEMEM_API void transformObjectAddresses(void* object, const TypeName& typeName, bool recurseFields, std::set<void*>* recursePointers) const;

	ENGINEMEM_API void clear();
};
