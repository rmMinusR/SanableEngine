#include "TypeName.hpp"

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

TypeName::TypeName(const std::string& unwrappedTypeName, int ptrDepth) :
    unwrappedTypeName(unwrappedTypeName),
    nameHash(makeHash(unwrappedTypeName)),
    ptrDepth(ptrDepth)
{
}
