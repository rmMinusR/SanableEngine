#include "Resources.hpp"

#define _X(type, name) type Resources::name;
FOREACH_RESOURCE()
#undef _X
