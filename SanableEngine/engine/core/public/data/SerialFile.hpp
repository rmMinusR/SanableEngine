#pragma once

#include <filesystem>
#include "dllapi.h"

class SerialNode;

class SerialFile
{
	SerialNode* rootNode;
public:
	ENGINEDATA_API SerialFile(const std::filesystem::path&);
	ENGINEDATA_API ~SerialFile();
	ENGINEDATA_API SerialNode const* getRootNode() const;
};
