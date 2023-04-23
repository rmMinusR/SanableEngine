#pragma once

#include "Asset.hpp"
#include "dllapi.h"

class TTFModule : public Asset
{
protected:
	virtual void loadInternal() override;
	virtual void unloadInternal() override;

	TTFModule() = default;
public:
	ENGINEASSETS_API static TTFModule instance;
};
