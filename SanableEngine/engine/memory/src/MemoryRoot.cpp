#include "MemoryRoot.hpp"

#include <cassert>

#include "MemoryHeap.hpp"

std::optional<MemoryRoot> MemoryRoot::instance;

MemoryRoot* MemoryRoot::get()
{
	if (!MemoryRoot::instance.has_value()) MemoryRoot::instance.emplace();
	return &MemoryRoot::instance.value();
}

void MemoryRoot::cleanup()
{
	MemoryRoot::instance.reset();
}

MemoryRoot::MemoryRoot()
{
	assert(!instance.has_value());
}

MemoryRoot::~MemoryRoot()
{
}

void MemoryRoot::visitHeaps(const std::function<void(MemoryHeap*)>& visitor)
{
	for (MemoryHeap* heap : livingHeaps) visitor(heap);
}

void MemoryRoot::updatePointers(const MemoryMapper& remapper, std::set<void*>& visitRecord)
{
	for (MemoryHeap* heap : livingHeaps) heap->updatePointers(remapper, visitRecord);
	externals_updatePointers(remapper, visitRecord);
}

void MemoryRoot::externals_updatePointers(const MemoryMapper& remapper, std::set<void*>& visitRecord)
{
	for (const auto& it : externalObjects)
	{
		const TypeInfo* ty = std::get<0>(it.second).resolve();
		ExternalObjectOptions opts = std::get<2>(it.second);
		if (ty)
		{
			remapper.transformObjectAddresses(
				it.first,
				ty,
				(uint8_t)opts&(uint8_t)ExternalObjectOptions::AllowFieldRecursion,
				((uint8_t)opts&(uint8_t)ExternalObjectOptions::AllowPtrRecursion) ? &visitRecord : nullptr
			);
		}
	}
}

void MemoryRoot::registerHeap(MemoryHeap* heap)
{
	livingHeaps.emplace_back(heap);
}

void MemoryRoot::removeHeap(MemoryHeap* heap)
{
	auto it = std::find(livingHeaps.begin(), livingHeaps.end(), heap);
	assert(it != livingHeaps.end());
	livingHeaps.erase(it);
}

void MemoryRoot::registerExternal_impl(void* object, TypeName&& type, size_t size, ExternalObjectOptions options)
{
	typedef std::tuple<TypeName, size_t, ExternalObjectOptions> details_t;
	externalObjects.try_emplace(object, details_t(type, size, options));
}
