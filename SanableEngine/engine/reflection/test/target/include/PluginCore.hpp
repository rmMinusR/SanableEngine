#pragma once

#include "ModuleTypeRegistry.inl"

//
// DUMMY FILE to make generated RTTI play nice
//

#define PLUGIN_C_API(returnVal) returnVal
PLUGIN_C_API(void) plugin_reportTypes(ModuleTypeRegistry* report);
