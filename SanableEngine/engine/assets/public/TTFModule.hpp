#pragma once

#include "Asset.hpp"
#include "dllapi.h"

class TTFModule : public Asset
{
protected:
	virtual void loadInternal(MemoryManager*) override;
	virtual void unloadInternal(MemoryManager*) override;

	TTFModule() = default;
public:
	ENGINEASSETS_API static TTFModule instance;
};
