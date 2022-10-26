#pragma once

#pragma once

#include <vector>
#include <map>
#include <cassert>
#include <ostream>

#include "RawMemoryPool.hpp"

class MemoryPoolPagedResizing
{
private:
    static constexpr size_t POOL_OBJ_COUNT = 16;
    static constexpr size_t POOL_STARTING_COUNT = 0;
    std::map<size_t, std::vector<RawMemoryPool*>> poolGroups;
    
    static constexpr size_t fitSize(size_t raw); //For choosing which pool objects go into
    static constexpr size_t calcWaste(size_t raw);

    size_t alignWasted; //Space wasted due to smaller objects going in larger pools. With a log2 setup, should never be 50% or more.

    bool isAlive;

public:
    MemoryPoolPagedResizing();
    ~MemoryPoolPagedResizing();

    //Idiotproofing against myself
    MemoryPoolPagedResizing(const MemoryPoolPagedResizing&) = delete;
    MemoryPoolPagedResizing(MemoryPoolPagedResizing&&) = default;

    void init();
    void cleanup();

    void reset();

    size_t getTotalCapacity() const;
    size_t getTotalAllocated() const;

    size_t getTotalWaste() const;

    //Allocates raw memory. NOT RECOMMENDED because we can't find the size of
    //a void pointer, meaning this must be tracked separately for us to find
    //the right memory pool within reasonable time.
    void* allocate(const size_t& size);

    //Deallocates raw memory. NOT RECOMMENDED because we can't find the size of
    //a void pointer, meaning this must be tracked separately for us to find
    //the right memory pool within reasonable time. Also not recommended because
    //it doesn't call destructor.
    void deallocate(void* obj, size_t size);

    //Allocates memory and creates an object. RECOMMENDED because it has decent
    //type safety and data safety.
    template<typename TObj, typename... TCtorArgs>
    TObj* emplace(const TCtorArgs&... ctorArgs) {
        TObj* pObj = reinterpret_cast<TObj*>(allocate(sizeof(TObj)));
        assert(pObj);
        //Construct object
        new (pObj) TObj(ctorArgs...);
        return pObj;
    }

    template<typename TObj>
    void free(TObj* obj) {
        //Manual dtor call since we didn't call delete/memfree
        optional_destructor<TObj>::call(obj);

        deallocate(obj, sizeof(TObj));
    }

    void debugReport(std::ostream& out) const;
};
