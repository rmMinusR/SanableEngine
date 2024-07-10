#pragma once

struct PtrToInt
{
	int* target;
};

struct DoublePtrToInt
{
	int** target;
};

struct PtrToStruct
{
	PtrToInt* target;
};

struct CylicalPtrsLinear
{
	CylicalPtrsLinear* target;
};

struct CylicalPtrsTree
{
	CylicalPtrsTree* a;
	CylicalPtrsTree* b;
};

class ModuleTypeRegistry;
void test_reportTypes(ModuleTypeRegistry* registry);
