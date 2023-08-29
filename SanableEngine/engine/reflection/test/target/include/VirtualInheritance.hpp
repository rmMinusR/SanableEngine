#pragma once

struct VirtualSharedBase
{
	virtual ~VirtualSharedBase() = default;
};

struct VirtualInheritedA : public virtual VirtualSharedBase
{
	virtual ~VirtualInheritedA() = default;
};

struct VirtualInheritedB : public virtual VirtualSharedBase
{
	virtual ~VirtualInheritedB() = default;
};

struct VirtualDiamond : public virtual VirtualInheritedA, public VirtualInheritedB
{
	virtual ~VirtualDiamond() = default;
};
