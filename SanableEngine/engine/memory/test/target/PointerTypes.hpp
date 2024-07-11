#pragma once

struct PtrToInt
{
	int* target = nullptr;
};

struct DoublePtrToInt
{
	int** target = nullptr;
};

struct PtrToStruct
{
	PtrToInt* target = nullptr;
};

struct CylicalPtrsLinear
{
	CylicalPtrsLinear* target = nullptr;
};

struct CylicalPtrsTree
{
	CylicalPtrsTree* a = nullptr;
	CylicalPtrsTree* b = nullptr;
};

class ModuleTypeRegistry;
void test_reportTypes(ModuleTypeRegistry* registry);
