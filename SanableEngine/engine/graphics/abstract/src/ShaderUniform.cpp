#include "ShaderUniform.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "ShaderProgram.hpp"

const char* ShaderUniform::ValueBinding_getName(ValueBinding binding)
{
	switch (binding)
	{
#define _X(val) case ValueBinding::val: return #val;
	ValueBinding_VALUES_ALL
#undef _X

	default: return nullptr;
	}
}

ShaderUniform::ValueBinding ShaderUniform::ValueBinding_fromName(const std::string& name)
{
#define _X(val) if (std::strncmp(name.c_str(), #val, std::max(name.size(), strlen(#val))) == 0) return ValueBinding::val;
	ValueBinding_VALUES_ALL
#undef _X

	return (ValueBinding)-1;
}

ShaderUniform::BindingStage ShaderUniform::BindingStage_fromBinding(ValueBinding binding)
{
	if (binding == ValueBinding::Unbound) return BindingStage::Unbound;
	else if ((int)ValueBinding::__BEGIN_SHARED < (int)binding && (int)binding < (int)ValueBinding::__END_SHARED) return BindingStage::BindShared;
	else if ((int)ValueBinding::__BEGIN_INSTANCED < (int)binding && (int)binding < (int)ValueBinding::__END_INSTANCED) return BindingStage::BindInstanced;
	else
	{
		assert(false);
		return (BindingStage)-1;
	}
}

ShaderUniform::ShaderUniform() :
	owner(nullptr),
	binding(ValueBinding::Invalid)
{
}

ShaderUniform::ShaderUniform(ShaderProgram* owner) :
	owner(owner),
	binding(ValueBinding::Invalid)
{
}

ShaderUniform::~ShaderUniform()
{
}

ShaderUniform::ValueBinding ShaderUniform::getBinding() const
{
	return binding;
}

ShaderProgram* ShaderUniform::getOwner() const
{
	return owner;
}
