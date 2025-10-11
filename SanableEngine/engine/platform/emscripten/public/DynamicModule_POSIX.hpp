#pragma once

#include "DynamicModule.hpp"


namespace gpr460 { class System_Emscripten; }


class DynamicModule_POSIX : public DynamicModule
{
public:
	virtual ~DynamicModule_POSIX();

	virtual void* getSymbol(const char* name) const;

protected:
	friend class gpr460::System_Emscripten;
	DynamicModule_POSIX(std::filesystem::path path);

	void* sharedObjectHandle;
};
