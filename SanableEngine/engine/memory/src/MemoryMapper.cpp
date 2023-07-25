#include "MemoryMapper.hpp"

#include <cstring>
#include <cassert>

void MemoryMapper::move(void* dst, void* src, size_t bytesToMove)
{
	memcpy(dst, src, bytesToMove); //Do actual move op
	if (USE_INVALID_DATA_FILL) memset(src, INVALID_DATA_FILL_VALUE, bytesToMove); //Fill old memory

	logMove(dst, src, bytesToMove);
}

void MemoryMapper::logMove(void* dst, void* src, size_t bytesToMove)
{
	RemapOp op;
	op.src = src;
	op.dst = dst;
	op.blockSize = bytesToMove;
	opQueue.push_back(op);
}

void* MemoryMapper::transformAddress(void* ptr, size_t ptrSize) const
{
	for (const RemapOp& op : opQueue)
	{
		void* srcEnd = ((char*)op.src) + op.blockSize; //First byte NOT in remapped block
		if (op.src <= ptr && ptr < srcEnd)
		{
			void* lastByte = ((char*)ptr) + ptrSize - 1;
			assert(lastByte < srcEnd); //Ensure last byte is also in range. No object shearing!
			ptr = ((char*)ptr) - ((char*)op.src) + ((char*)op.dst);
		}
	}

	return ptr;
}
