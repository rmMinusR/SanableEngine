#include "SerialNode.hpp"

#include <sstream>

#include <cassert>
#include <cwctype>

void skipWhitespace(std::wistream& in)
{
	while (std::iswspace(in.get()));
	in.unget(); //Correct for over-read
}

SerialNode* SerialNode::parseYaml(std::wistream& in, const wchar_t* indent)
{
	if (indent == nullptr) indent = L"";

	return nullptr; //TODO
}

std::wstring jsonCaptureString(std::wistream& in)
{
	wchar_t opener = in.get();
	assert(opener == L'"' || opener == L'\'');

	std::wostringstream out;
	wchar_t pv = 0;
	wchar_t v = 0;
	while (true)
	{
		pv = v;
		v = in.get();
		
		if (v == opener && pv != L'\\')
		{
			//in.unget(); //No need to unget, since we just consumed the closing '"'
			return out.str();
		}
		else
		{
			out << v;
		}
	}
}

SerialNode* SerialNode::parseJson(std::wistream& in)
{
	skipWhitespace(in);
	wchar_t inType = in.peek();

	     if (inType == L'{')                    return SerialObject::parseJson(in);
	else if (inType == L'[')                    return SerialArray ::parseJson(in);
	else if (inType == L'"' || inType == L'\'') return SerialString::parseJson(in);
	else if (std::iswdigit(inType))             return SerialNumber::parseJson(in);
	else //Don't know what we're parsing
	{
		assert(false);
		return nullptr;
	}
}

SerialString* SerialString::parseJson(std::wistream& in)
{
	return new SerialString(jsonCaptureString(in));
}

SerialNumber* SerialNumber::parseJson(std::wistream& in)
{
	std::wostringstream buf;
	wchar_t v = 0;
	bool isDecimal = false;

readNext:
	v = in.get();
	if (std::iswdigit(v) || v == L'.')
	{
		isDecimal |= (v == L'.');
		buf << v;
		goto readNext;
	}

	in.unget(); //Prevent over-reading

	SerialNumber::contents_t out;
	if (isDecimal) out = std::stof(buf.str());
	else		   out = std::stoi(buf.str());
	return new SerialNumber(out);
}

SerialObject* SerialObject::parseJson(std::wistream& in)
{
	wchar_t header = in.get();
	assert(header == L'{');

	skipWhitespace(in); //Seek to next value

	SerialObject::contents_t contents;
parseNext:
	std::wstring key = jsonCaptureString(in);
	skipWhitespace(in); //Seek to ':'
	wchar_t sep = in.get(); //Skip ':'
	assert(sep == L':');
	SerialNode* val = SerialNode::parseJson(in);
	contents.emplace(key, val); //Recurse for value

	skipWhitespace(in); //Seek to next control character
	wchar_t delim = in.get();
	if (delim == L',')
	{
		skipWhitespace(in); //Seek to next value
		goto parseNext; //Keep parsing
	}
	else if (delim == L'}') return new SerialObject(contents);
	else
	{
		//Unknown control character
		assert(false);
		return nullptr;
	}

	return new SerialObject(contents);
}

SerialArray* SerialArray::parseJson(std::wistream& in)
{
	wchar_t header = in.get();
	assert(header == L'[');

	skipWhitespace(in); //Seek to next value

	SerialArray::contents_t contents;
parseNext:
	contents.push_back(SerialNode::parseJson(in)); //Recurse for value

	skipWhitespace(in); //Seek to next control character
	wchar_t delim = in.get();
	if (delim == L',')
	{
		skipWhitespace(in); //Seek to next value
		goto parseNext; //Keep parsing
	}
	else if (delim == L']') return new SerialArray(contents);
	else
	{
		//Unknown control character
		assert(false);
		return nullptr;
	}
}
