#include "MemoryManager.hpp"

#include <cmath>
#include <cassert>
#include <type_traits>

//From: https://stackoverflow.com/a/17728525
constexpr int64_t ipow(int64_t base, int exp, int64_t result = 1) {
	return exp < 1 ? result : ipow(base * base, exp / 2, (exp % 2) ? result * base : result);
}

constexpr size_t MemoryManager::fitSize(size_t raw)
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

constexpr size_t MemoryManager::calcWaste(size_t raw)
{
	return fitSize(raw) - raw;
}

MemoryManager::MemoryManager() :
	poolGroups(),
	alignWasted(0),
	isAlive(false)
{

}

MemoryManager::~MemoryManager()
{
	assert(!isAlive);
}

void MemoryManager::init()
{
	assert(!isAlive);
	isAlive = true;

	for (int i = 0; i < POOL_STARTING_COUNT; ++i) {
		std::vector<MemoryPoolForked*>& v = poolGroups.emplace(size_t(1 << i), std::vector<MemoryPoolForked*>()).first->second;
		v.push_back(new MemoryPoolForked(POOL_OBJ_COUNT, 1 << i));
	}
	
	alignWasted = 0;
}

void MemoryManager::cleanup()
{
	assert(isAlive);
	isAlive = false;

	for (std::pair<const size_t, std::vector<MemoryPoolForked*>>& group : poolGroups) {
		for(const MemoryPoolForked* pool : group.second) delete pool;
	}
	
	poolGroups.clear();
	alignWasted = 0;
}

void MemoryManager::reset()
{
	assert(isAlive);

	for (std::pair<const size_t, std::vector<MemoryPoolForked*>>& group : poolGroups) {
		for (MemoryPoolForked* pool : group.second) pool->reset();
	}
	alignWasted = 0;
}

size_t MemoryManager::getTotalCapacity() const
{
	assert(isAlive);

	size_t total = 0;
	for (const std::pair<const size_t, std::vector<MemoryPoolForked*>>& group : poolGroups) {
		//for (const MemoryPoolForked* pool : group.second) total += group.first * POOL_OBJ_COUNT;
		total += group.second.size() * group.first * POOL_OBJ_COUNT;
	}
	return total;
}

size_t MemoryManager::getTotalAllocated() const
{
	assert(isAlive);

	size_t total = 0;
	for (const std::pair<const size_t, std::vector<MemoryPoolForked*>>& group : poolGroups) {
		for (const MemoryPoolForked* pool : group.second) total += pool->getMaxObjectSize() * pool->getNumAllocatedObjects();
	}
	return total;
}

size_t MemoryManager::getTotalWaste() const
{
	assert(isAlive);

	//If we were allocating back-to-back we'd need to account for fragmentation
	//caused by shifting alignment, but alignment is fixed here so there's no need
	return alignWasted;
}

void* MemoryManager::allocate(const size_t& size)
{
    //"Get or create" idiom. Basically a null-coalesced construction.
	//pools[...] ?? (pools[...] = new MemoryPoolForked(...))
    size_t fs = fitSize(size);

    auto groupIt = poolGroups.find(fs);
	std::vector<MemoryPoolForked*>& group = (groupIt != poolGroups.end())          //Search for the appropriate memory pool group
        ? groupIt->second                                                         //If a matching group exists, use it
        : (poolGroups.emplace(fs, std::vector<MemoryPoolForked*>()).first->second); //Only create if no appropriate group found

	//Loop through pools until we find one that can hold our object.
	int poolID = 0;
	void* obj = nullptr;
	do {
		if (group.size() < poolID + 1) {
			obj = group.emplace_back(new MemoryPoolForked(POOL_OBJ_COUNT, fs))->allocateObject(); //No pool exists that can hold our object? Create one.
			break;
		}
		obj = group[poolID]->allocateObject();
		++poolID;
	} while (!obj);

	alignWasted += fs - size; //Track wasted memory

    return obj;
}

void MemoryManager::deallocate(void* obj, size_t size)
{
	assert(obj);
	if(obj) alignWasted -= fitSize(size) - size;

    auto it = poolGroups.find(fitSize(size));
    assert(it != poolGroups.end());
    
	//Search for the pool that contains this object
	MemoryPoolForked* ownerPool = nullptr;
	for (MemoryPoolForked* pool : it->second) {
		if (pool->contains(obj)) {
			ownerPool = pool;
			break;
		}
	}

	assert(ownerPool);
	ownerPool->freeObject(obj);
}

void MemoryManager::debugReport(std::ostream& out) const
{
	size_t waste = getTotalWaste();
	float wastePercent = waste > 0 ? (float(waste) / getTotalAllocated() * 100) : 0;
	out << "MemoryManager@" << this << ": " << getTotalAllocated() << " of " << getTotalCapacity() << " bytes used / " << waste << " wasted (" << wastePercent << "%)";
}
