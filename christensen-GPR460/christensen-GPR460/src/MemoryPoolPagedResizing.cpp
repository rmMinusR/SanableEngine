#include "MemoryPoolPagedResizing.hpp"

#include <cmath>
#include <cassert>
#include <type_traits>

//From: https://stackoverflow.com/a/17728525
constexpr int64_t ipow(int64_t base, int exp, int64_t result = 1) {
	return exp < 1 ? result : ipow(base * base, exp / 2, (exp % 2) ? result * base : result);
}

constexpr size_t MemoryPoolPagedResizing::fitSize(size_t raw)
{
	if (!std::is_constant_evaluated())
	{
		return 1 << (unsigned long)ceil(log2(raw));
	}
	else
	{
		size_t r = raw;
		size_t p = 0;
		while (r != 0b1) {
			r >>= 1;
			++p;
		}
		if (r != (1<<p)) return p + 1;
		else return p;
	}
	//return sizeof(T);
}

constexpr size_t MemoryPoolPagedResizing::calcWaste(size_t raw)
{
	return fitSize(raw) - raw;
}

MemoryPoolPagedResizing::MemoryPoolPagedResizing() :
	poolGroups(),
	alignWasted(0),
	isAlive(false)
{

}

MemoryPoolPagedResizing::~MemoryPoolPagedResizing()
{
	assert(!isAlive);
}

void MemoryPoolPagedResizing::init()
{
	assert(!isAlive);
	isAlive = true;

	for (int i = 0; i < POOL_STARTING_COUNT; ++i) {
		std::vector<MemoryPoolFixedSize*>& v = poolGroups.emplace(size_t(1 << i), std::vector<MemoryPoolFixedSize*>()).first->second;
		v.push_back(new MemoryPoolFixedSize(POOL_OBJ_COUNT, 1 << i));
	}
	
	alignWasted = 0;
}

void MemoryPoolPagedResizing::cleanup()
{
	assert(isAlive);
	isAlive = false;

	for (std::pair<const size_t, std::vector<MemoryPoolFixedSize*>>& group : poolGroups) {
		for(const MemoryPoolFixedSize* pool : group.second) delete pool;
	}
	
	poolGroups.clear();
	alignWasted = 0;
}

void MemoryPoolPagedResizing::reset()
{
	assert(isAlive);

	for (std::pair<const size_t, std::vector<MemoryPoolFixedSize*>>& group : poolGroups) {
		for (MemoryPoolFixedSize* pool : group.second) pool->reset();
	}
	alignWasted = 0;
}

size_t MemoryPoolPagedResizing::getTotalCapacity() const
{
	assert(isAlive);

	size_t total = 0;
	for (const std::pair<const size_t, std::vector<MemoryPoolFixedSize*>>& group : poolGroups) {
		//for (const MemoryPoolForked* pool : group.second) total += group.first * POOL_OBJ_COUNT;
		total += group.second.size() * group.first * POOL_OBJ_COUNT;
	}
	return total;
}

size_t MemoryPoolPagedResizing::getTotalAllocated() const
{
	assert(isAlive);

	size_t total = 0;
	for (const std::pair<const size_t, std::vector<MemoryPoolFixedSize*>>& group : poolGroups) {
		for (const MemoryPoolFixedSize* pool : group.second) total += pool->getMaxObjectSize() * pool->getNumAllocatedObjects();
	}
	return total;
}

size_t MemoryPoolPagedResizing::getTotalWaste() const
{
	assert(isAlive);

	//If we were allocating back-to-back we'd need to account for fragmentation
	//caused by shifting alignment, but alignment is fixed here so there's no need
	return alignWasted;
}

void* MemoryPoolPagedResizing::allocate(const size_t& size)
{
    //"Get or create" idiom. Basically a null-coalesced construction.
	//pools[...] ?? (pools[...] = new MemoryPoolForked(...))
    size_t fs = fitSize(size);

    auto groupIt = poolGroups.find(fs);
	std::vector<MemoryPoolFixedSize*>& group = (groupIt != poolGroups.end())          //Search for the appropriate memory pool group
        ? groupIt->second                                                         //If a matching group exists, use it
        : (poolGroups.emplace(fs, std::vector<MemoryPoolFixedSize*>()).first->second); //Only create if no appropriate group found

	//Loop through pools until we find one that can hold our object.
	int poolID = 0;
	void* obj = nullptr;
	do {
		if (group.size() < poolID + 1) {
			obj = group.emplace_back(new MemoryPoolFixedSize(POOL_OBJ_COUNT, fs))->allocate(); //No pool exists that can hold our object? Create one.
			break;
		}
		obj = group[poolID]->allocate();
		++poolID;
	} while (!obj);

	alignWasted += fs - size; //Track wasted memory

    return obj;
}

void MemoryPoolPagedResizing::deallocate(void* obj, size_t size)
{
	assert(obj);
	if(obj) alignWasted -= fitSize(size) - size;

    auto it = poolGroups.find(fitSize(size));
    assert(it != poolGroups.end());
    
	//Search for the pool that contains this object
	MemoryPoolFixedSize* ownerPool = nullptr;
	for (MemoryPoolFixedSize* pool : it->second) {
		if (pool->contains(obj)) {
			ownerPool = pool;
			break;
		}
	}

	assert(ownerPool);
	ownerPool->free(obj);
}

void MemoryPoolPagedResizing::debugReport(std::ostream& out) const
{
	size_t waste = getTotalWaste();
	float wastePercent = waste > 0 ? (float(waste) / getTotalAllocated() * 100) : 0;
	out << "MemoryManager@" << this << ": " << getTotalAllocated() << " of " << getTotalCapacity() << " bytes used / " << waste << " wasted (" << wastePercent << "%)";
}
