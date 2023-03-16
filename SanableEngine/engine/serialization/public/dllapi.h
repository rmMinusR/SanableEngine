//Deliberately no include guards

#if engine_serialization_EXPORTS
#define ENGINESERIALIZATION_API __declspec(dllexport)
#else
#define ENGINESERIALIZATION_API __declspec(dllimport)
#endif