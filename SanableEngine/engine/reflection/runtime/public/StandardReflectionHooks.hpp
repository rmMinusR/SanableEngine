
#define SANABLE_REFLECTION_REGISTRATION_POINT
#define SANABLE_REFLECTION_HOOKS

/*

class ModuleTypeRegistry;

#ifndef SANABLE_REFLECTION_REGISTRATION_POINT
#define SANABLE_REFLECTION_REGISTRATION_POINT void __declspec(dllexport) __cdecl plugin_reportTypes(ModuleTypeRegistry* registry);
//Forward declare dependencies
extern "C" SANABLE_REFLECTION_REGISTRATION_POINT;
#endif



#ifndef SANABLE_REFLECTION_HOOKS
#define SANABLE_REFLECTION_HOOKS friend SANABLE_REFLECTION_REGISTRATION_POINT;\
								 friend class TypeBuilder;
//Forward declare dependencies
class TypeBuilder;
#endif

*/
