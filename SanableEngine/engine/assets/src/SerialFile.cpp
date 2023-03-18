#include "SerialFile.hpp"

#include <fstream>

SerialFile::SerialFile(const std::filesystem::path& path) :
	FileAsset(path),
	rootNode(nullptr)
{
}

SerialNode const* SerialFile::getRootNode() const
{
	return rootNode;
}

void SerialFile::loadInternal()
{
	std::wifstream in;
	in.open(path);
	assert(in.good());
	rootNode = SerialNode::parse(in, SerialFormat::JSON); //TODO autodetect format
	in.close();
}

void SerialFile::unloadInternal()
{
	delete rootNode;
	rootNode = nullptr;
}