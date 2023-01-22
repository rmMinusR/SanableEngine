//Deliberately no include guards

#include "PluginCore.hpp"

#if PrimitivesPlugin_EXPORTS
#define PRIMITIVES_API __declspec(dllexport) PLUGIN_API_KEEPALIVE
#else
#define PRIMITIVES_API __declspec(dllimport) PLUGIN_API_KEEPALIVE
#endif