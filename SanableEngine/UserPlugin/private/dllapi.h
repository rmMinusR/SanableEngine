//Deliberately no include guards

#include "PluginCore.hpp"

#if UserPlugin_EXPORTS
#define USERPLUGIN_API API_EXPORT API_KEEPALIVE
#else
#define USERPLUGIN_API API_IMPORT API_KEEPALIVE
#endif