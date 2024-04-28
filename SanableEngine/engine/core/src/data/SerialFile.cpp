#include "data/SerialFile.hpp"

#include <fstream>
#include <cassert>
#include "data/SerialNode.hpp"

SerialFile::SerialFile(const std::filesystem::path& path) :
	rootNode(nullptr)
{
	std::wifstream in;
	in.open(path);
	assert(in.good());
	rootNode = SerialNode::parse(in, SerialFormat::JSON); //TODO autodetect format
	in.close();
}

SerialFile::~SerialFile()
{
	if (rootNode) delete rootNode;
}

SerialNode const* SerialFile::getRootNode() const
{
	return rootNode;
}
