#pragma once

#include <vector>
#include <cassert>

#include "dllapi.h"
#include "TypeName.hpp"


template<typename TRet, typename... TArgs>
struct FunctionUtil
{
	typedef TRet(__cdecl *fnptr_cdecl)(TArgs...);
	typedef TRet(__stdcall *fnptr_stdcall)(TArgs...);

	template<typename TOwner>
	struct Member
	{
		typedef TRet(__thiscall TOwner::*fnptr_thiscall)(TArgs...);
		//TODO deal with functions where "this" is const
	};
};


class FunctionInfo
{
public:
	enum class CallConv
	{
		Invalid = 0,
		CDecl,
		StdCall,
		ThisCall
	};

	ENGINE_RTTI_API FunctionInfo();
	ENGINE_RTTI_API ~FunctionInfo();
	ENGINE_RTTI_API FunctionInfo(const FunctionInfo& cpy);
	ENGINE_RTTI_API FunctionInfo& operator=(const FunctionInfo& cpy);
	ENGINE_RTTI_API FunctionInfo(FunctionInfo&& mov);
	ENGINE_RTTI_API FunctionInfo& operator=(FunctionInfo&& mov);

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
	TypeName owningType; //Only if nonstatic member function
	CallConv callConv;

	#pragma region Capture functions and helpers

private:
	//Type erasure mechanism
	template<int _, typename Head, typename... Tail>
	inline static void identifyArgs(std::vector<TypeName>& out)
	{
		out.push_back(TypeName::create<Head>()))
		identifyArgs<_, Tail...>(out);
	}
	template<int _>
	inline static void identifyArgs(std::vector<TypeName>& out) { }

	template<typename TRet, typename... TArgs>
	static FunctionInfo captureCommon()
	{
		FunctionInfo out;
		out.returnType = TypeName::create<TRet>();
		identifyArgs<0, TArgs...>(out.args);
		return out;
	}
public:
	template<typename TRet, typename... TArgs>
	static FunctionInfo captureCDecl(TRet(__cdecl* fn)(TArgs...))
	{
		FunctionInfo out = captureCommon<TRet, TArgs...>();
		out.callConv = CallConv::CDecl;
		assert(out.isValid());
		return out;
	}

	template<typename TRet, typename... TArgs>
	static FunctionInfo captureStdCall(TRet(__stdcall* fn)(TArgs...))
	{
		FunctionInfo out = captureCommon<TRet, TArgs...>();
		out.callConv = CallConv::StdCall;
		assert(out.isValid());
		return out;
	}

	template<typename TOwner, typename TRet, typename... TArgs>
	static FunctionInfo captureThisCall(TRet(__thiscall TOwner::*fn)(TArgs...))
	{
		static_assert(false); //TODO fix storage: Member ptrs are sometimes wider
		FunctionInfo out = captureCommon<TRet, TArgs...>();
		out.owningType = TypeName::create<TOwner>();
		out.callConv = CallConv::StdCall;
		assert(out.isValid());
		return out;
	}

	#pragma endregion
};