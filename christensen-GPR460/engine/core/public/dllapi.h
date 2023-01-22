//Deliberately no include guards

#include "PluginCore.hpp"

#if engine_core_EXPORTS
#define ENGINECORE_API __declspec(dllexport) PLUGIN_API_KEEPALIVE
#else
#define ENGINECORE_API __declspec(dllimport) PLUGIN_API_KEEPALIVE
#endif