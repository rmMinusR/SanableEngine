#include "TypeName.hpp"

#include <cassert>

#include "GlobalTypeRegistry.hpp"

TypeName::hash_t TypeName::makeHash(const std::string& str)
{
    //DJB2 string hash
    hash_t hash = 5381;
        
    for (int i = 0; i < str.length(); ++i)
    {
        //hash = ((hash << 5) + hash) + c;
        hash = hash * 33 ^ str[i];
    }

    return hash;
}

TypeName::TypeName() :
    unwrappedTypeName(""),
    nameHash(0),
    ptrDepth(0)
{
}

TypeName::TypeName(const std::string& unwrappedTypeName) :
    TypeName(unwrappedTypeName, 0)
{
}

TypeName::TypeName(const std::string& unwrappedTypeName, int ptrDepth) :
    unwrappedTypeName(unwrappedTypeName), //FIXME unprefix ("class MyClass")
    nameHash(makeHash(unwrappedTypeName)),
    ptrDepth(ptrDepth)
{
}

TypeName TypeName::parse(const std::string& unsafeName)
{
    //TODO implement stub
    return TypeName(unsafeName);
}

bool TypeName::isValid() const
{
    return !unwrappedTypeName.empty();
}

TypeInfo const* TypeName::resolve() const
{
    return GlobalTypeRegistry::lookupType(*this);
}

bool TypeName::operator==(const TypeName& other) const
{
    //Compare the stuff that's easy before doing a full string compare
    return ptrDepth == other.ptrDepth
        && nameHash == other.nameHash
        && unwrappedTypeName == other.unwrappedTypeName;
}

bool TypeName::operator!=(const TypeName& other) const
{
    //Compare the stuff that's easy before doing a full string compare
    return ptrDepth != other.ptrDepth
        || nameHash != other.nameHash
        || unwrappedTypeName != other.unwrappedTypeName;
}

const std::string& TypeName::as_str() const
{
    assert(isValid());
    return unwrappedTypeName;
}

char const* TypeName::c_str() const
{
    assert(isValid());
    return unwrappedTypeName.c_str();
}
