//Deliberately no include guards

#if engine_core_EXPORTS
#define ENGINECORE_API __declspec(dllexport)
#else
#define ENGINECORE_API __declspec(dllimport)
#endif