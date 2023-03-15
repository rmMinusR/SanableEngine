#include "SerialFile.hpp"

#include <fstream>

SerialFile::SerialFile(const std::filesystem::path& path) :
	FileAsset(path),
	rootNode(nullptr)
{
}

void SerialFile::loadInternal(MemoryManager* mem)
{
	std::wifstream in;
	in.open(path);
	assert(in.good());
	rootNode = SerialNode::parse(in, SerialFormat::JSON); //TODO autodetect format
	in.close();
}

void SerialFile::unloadInternal(MemoryManager* mem)
{
	delete rootNode;
	rootNode = nullptr;
}