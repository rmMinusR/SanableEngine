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
	SerialFile(const std::filesystem::path&);
	inline SerialNode const* getRootNode() const { return rootNode; }
};