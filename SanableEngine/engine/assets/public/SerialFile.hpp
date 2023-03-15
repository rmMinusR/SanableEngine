#pragma once

#include "FileAsset.hpp"
#include "SerialNode.hpp"

class SerialFile : public FileAsset
{
protected:
	virtual void loadInternal(MemoryManager*) override;
	virtual void unloadInternal(MemoryManager*) override;

	SerialNode* rootNode;
public:
	inline SerialNode const* getRootNode() const { return rootNode; }
};