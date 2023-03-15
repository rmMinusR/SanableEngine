//Deliberately no include guards

#if _WIN32
#define API_EXPORT __declspec(dllexport)
#define API_IMPORT __declspec(dllimport)
#define API_KEEPALIVE
#endif

#if __EMSCRIPTEN__
#include <emscripten.h>
#define API_EXPORT
#define API_IMPORT
#define API_KEEPALIVE EMSCRIPTEN_KEEPALIVE
#endif

#if engine_graphics_EXPORTS
#define ENGINEGRAPHICS_API API_EXPORT API_KEEPALIVE
#else
#define ENGINEGRAPHICS_API API_IMPORT API_KEEPALIVE
#endif
