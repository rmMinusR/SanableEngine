//Deliberately no include guards

#include "application/PluginCore.hpp"

#if PrimitivesPlugin_EXPORTS
#define PRIMITIVES_API API_EXPORT API_KEEPALIVE
#else
#define PRIMITIVES_API API_IMPORT API_KEEPALIVE
#endif