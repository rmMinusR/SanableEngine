//Deliberately no include guards

#if _WIN32
#define API_KEEPALIVE
#endif

#if __EMSCRIPTEN__
#include <emscripten.h>
#define API_KEEPALIVE EMSCRIPTEN_KEEPALIVE
#endif

#if engine_graphics_abstract_EXPORTS
#define ENGINEGRAPHICS_API API_KEEPALIVE
#else
#define ENGINEGRAPHICS_API API_KEEPALIVE
#endif
