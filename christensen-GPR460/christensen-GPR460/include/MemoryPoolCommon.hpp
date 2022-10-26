#pragma once

#include <type_traits>
#include <cassert>

#pragma region Destructor template magick

namespace HelperWrappers {
    template<typename TObj, bool has_destructor>
    struct optional_destructor {
        static void call(TObj* obj);

    private:
        optional_destructor() = delete;
    };

    template<typename TObj>
    struct optional_destructor<TObj, true> {
        static void call(TObj* obj) {
            //Dtor present, call it
            obj->~TObj();
        }
    };

    template<typename TObj>
    struct optional_destructor<TObj, false> {
        static void call(TObj* obj) {
            //No dtor present, nothing to do
        }
    };
}

template<typename TObj>
using optional_destructor = HelperWrappers::optional_destructor<TObj, std::is_destructible<TObj>::value>;

#pragma endregion


#pragma region RAII

template<typename TObj, typename TPool>
class PooledResource
{
public:
	~PooledResource() {
		pool->free(this);
	}

	PooledResource(PooledResource&& mov)
	{
		data = std::move(mov.data);
		pool = std::move(mov.pool);

		//Ensure we don't accidentally free by destroying the old guard object
		mov.data = nullptr;
		mov.pool = nullptr;
	}

	TObj* const operator*() {
		assert(data); //Ensure good
		return data;
	}

private:
	PooledResource(void* data, TPool* pool) :
		data(data),
		pool(pool)
	{
	}
	PooledResource(const PooledResource& cpy) = delete;

	void* data;
	TPool* pool;
};

#pragma endregion


#pragma region From original MemoryPool by Dean Lawson

int isPowerOfTwo(unsigned int x);
unsigned int getClosestPowerOf2LargerThan(unsigned int num);

#pragma endregion