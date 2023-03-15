#pragma once

#include "FileAsset.hpp"
#include "SerialNode.hpp"

#include "dllapi.h"

class SerialFile : public FileAsset
{
protected:
	virtual void loadInternal(MemoryManager*) override;
	virtual void unloadInternal(MemoryManager*) override;

	SerialNode* rootNode;
public:
	ENGINEASSETS_API SerialFile(const std::filesystem::path&);
	ENGINEASSETS_API SerialNode const* getRootNode() const;
};