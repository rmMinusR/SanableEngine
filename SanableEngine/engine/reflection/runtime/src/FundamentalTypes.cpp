#include "FundamentalTypes.hpp"

#include <type_traits>

#include "ModuleTypeRegistry.hpp"
#include "TypeBuilder.hpp"

template<typename T>
void fundamentalTypes_report_helper(ModuleTypeRegistry* r)
{
	static_assert(std::is_fundamental_v<T>);
	TypeBuilder builder = TypeBuilder::create<T>();
	builder.registerType(r);
}

void fundamentalTypes_reportRTTI(ModuleTypeRegistry* r)
{
	#define _X(ty) fundamentalTypes_report_helper<ty>(r);
	FOREACH_FUNDAMENTAL_TYPE()
	#undef _X
}

#define _X(ty) +1
size_t fundamentalTypes_names_sz = 0 FOREACH_FUNDAMENTAL_TYPE();
#undef _X

template<typename T> struct fundamentalTypes_names_lut_storage {};
#define _X(ty) template<> struct fundamentalTypes_names_lut_storage<ty> { static const char* cstr; }; const char* fundamentalTypes_names_lut_storage<ty>::cstr = #ty;
FOREACH_FUNDAMENTAL_TYPE()
#undef _X

#define _X(ty) fundamentalTypes_names_lut_storage<ty>::cstr,
const char* fundamentalTypes_names_storage[] = { FOREACH_FUNDAMENTAL_TYPE()};
#undef _X

const char** fundamentalTypes_names = fundamentalTypes_names_storage;
