#pragma once

#include "FileAsset.hpp"
#include "SerialNode.hpp"

#include "dllapi.h"

class SerialFile : public FileAsset
{
protected:
	virtual void loadInternal() override;
	virtual void unloadInternal() override;

	SerialNode* rootNode;
public:
	ENGINEASSETS_API SerialFile(const std::filesystem::path&);
	ENGINEASSETS_API SerialNode const* getRootNode() const;
};