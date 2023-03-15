#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <locale>
#include <cassert>
#include <variant>

enum class SerialFormat
{
	JSON,
	YAML
};

class SerialNode
{
protected:
	SerialNode() = default;

public:
	virtual ~SerialNode() = default;

	inline static SerialNode* parse(std::wistream& in, SerialFormat format)
	{
		switch (format)
		{
		case SerialFormat::YAML: return parseYaml(in);
		case SerialFormat::JSON: return parseJson(in);

		default:
			assert(false);
			return nullptr;
		}
	}

	static SerialNode* parseYaml(std::wistream& in, const wchar_t* indent = nullptr);
	static SerialNode* parseJson(std::wistream& in);
};

class SerialString : public SerialNode
{
public:
	typedef std::wstring contents_t;

private:
	contents_t contents;

public:
	inline SerialString(contents_t contents) : contents(contents) {}
	static SerialString* parseJson(std::wistream& in);
};

class SerialNumber : public SerialNode
{
public:
	typedef std::variant<float, int> contents_t;

private:
	contents_t contents;

public:
	inline SerialNumber(contents_t contents) : contents(contents) {}
	static SerialNumber* parseJson(std::wistream& in);
};

class SerialObject : public SerialNode
{
public:
	typedef std::unordered_map<std::wstring, SerialNode*> contents_t;

private:
	contents_t contents;

public:
	inline SerialObject(contents_t contents) : contents(contents) {}
	static SerialObject* parseJson(std::wistream& in);
};

class SerialArray : public SerialNode
{
public:
	typedef std::vector<SerialNode*> contents_t;
	
private:
	contents_t contents;

public:
	inline SerialArray(contents_t contents) : contents(contents) {}
	static SerialArray* parseJson(std::wistream& in);
};
