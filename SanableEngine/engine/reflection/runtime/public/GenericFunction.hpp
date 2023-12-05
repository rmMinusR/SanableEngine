#pragma once

#include <vector>
#include <cassert>
#include <functional>

#include "dllapi.h"
#include "TypeName.hpp"
#include "TypeInfo.hpp"


struct FunctionUtil
{
	//Type erasure mechanism

	//Name-only version
	template<int _, typename Head, typename... Tail>
	inline static void identifyArgs(std::vector<TypeName>& out)
	{
		out.push_back(TypeName::create<Head>());
		identifyArgs<0, Tail...>(out);
	}
	template<int _>
	inline static void identifyArgs(std::vector<TypeName>& out) { }

	//Shallow-detail version
	template<int _, typename Head, typename... Tail>
	inline static void identifyArgs(std::vector<TypeInfo>& out)
	{
		out.push_back(TypeInfo::createDummy<Head>());
		identifyArgs<0, Tail...>(out);
	}
	template<int _>
	inline static void identifyArgs(std::vector<TypeInfo>& out) { }

};

class VM;

enum class CallConv
{
	Invalid = 0,
	CDecl,
	StdCall,
	ThisCall
};


class GenericFunction
{
	friend class VM;

public:
	ENGINE_RTTI_API GenericFunction();
	ENGINE_RTTI_API ~GenericFunction();
	ENGINE_RTTI_API GenericFunction(const GenericFunction& cpy);
	ENGINE_RTTI_API GenericFunction& operator=(const GenericFunction& cpy);
	ENGINE_RTTI_API GenericFunction(GenericFunction&& mov);
	ENGINE_RTTI_API GenericFunction& operator=(GenericFunction&& mov);

	ENGINE_RTTI_API bool isValid() const;
	ENGINE_RTTI_API CallConv getCallConv() const;
	ENGINE_RTTI_API TypeName getReturnType() const;
	ENGINE_RTTI_API size_t getArgCount() const;
	ENGINE_RTTI_API TypeName getArg(size_t index) const;

	ENGINE_RTTI_API bool isMemberFunction() const;
	ENGINE_RTTI_API TypeName getOwner() const;

private:
	TypeName returnType;
	std::vector<TypeName> args;
	TypeName owningType; //Only present if nonstatic member function. Implies __thiscall.
	CallConv callConv;

	#pragma region Capture functions and helpers

private:

	template<typename TRet, typename... TArgs>
	static GenericFunction captureCommon()
	{
		GenericFunction out;
		out.returnType = TypeName::create<TRet>();
		FunctionUtil::identifyArgs<0, TArgs...>(out.args);
		return out;
	}

	//ENGINE_RTTI_API CallConv deduceCallConv(void(*thunk)(), const std::vector<TypeName> args) const;
public:

	/// Due to template shortcomings, the caller is responsible for correctly selecting cdecl/stdcall

	//template<typename TRet, typename... TArgs>
	//static GenericFunction capture(TRet(*fn)(TArgs...))
	//{
	//	GenericFunction out = captureCommon<TRet, TArgs...>();
	//	auto lambda = [=](TArgs... args) { fn(args...); };
	//	out.callConv = deduceCallConv((void(*)())&lambda.operator(), out.args);
	//	assert(out.isValid());
	//	return out;
	//}

	template<typename TRet, typename... TArgs>
	static GenericFunction captureCDecl(TRet(__cdecl* fn)(TArgs...))
	{
		GenericFunction out = captureCommon<TRet, TArgs...>();
		out.callConv = CallConv::CDecl;
		assert(out.isValid());
		return out;
	}

	template<typename TRet, typename... TArgs>
	static GenericFunction captureStdCall(TRet(__stdcall* fn)(TArgs...))
	{
		GenericFunction out = captureCommon<TRet, TArgs...>();
		out.callConv = CallConv::StdCall;
		assert(out.isValid());
		return out;
	}

	template<typename TOwner, typename TRet, typename... TArgs>
	static GenericFunction captureThisCall(TRet(__thiscall TOwner::*fn)(TArgs...))
	{
		//TODO fix storage: Member ptrs are sometimes wider
		GenericFunction out = captureCommon<TRet, TArgs...>();
		out.owningType = TypeName::create<TOwner>();
		out.callConv = CallConv::ThisCall;
		assert(out.isValid());
		return out;
	}

	#pragma endregion
};