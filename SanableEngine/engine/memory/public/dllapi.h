//Deliberately no include guards

#if _WIN32
#define API_EXPORT __declspec(dllexport)
#define API_IMPORT __declspec(dllimport)
#define API_KEEPALIVE
#elif __EMSCRIPTEN__
#include <emscripten.h>
#define API_EXPORT
#define API_IMPORT
#define API_KEEPALIVE EMSCRIPTEN_KEEPALIVE
#elif __unix__
#define API_EXPORT __attribute__((visibility("default")))
#define API_IMPORT
#define API_KEEPALIVE
#else
#error Unknown platform: don't know how to export shared library symbols
#endif

#if engine_memory_EXPORTS
#define ENGINEMEM_API API_EXPORT API_KEEPALIVE
#else
#define ENGINEMEM_API API_IMPORT API_KEEPALIVE
#endif

//#define ENGINEMEM_API
