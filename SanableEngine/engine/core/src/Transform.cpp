#include "Transform.hpp"

#include <cassert>
#include <algorithm>

Transform::Transform() :
	isDirtySelf(false),
	parent(nullptr),
	local(),
	global()
{
}

Transform::~Transform()
{
	//Remove ref from parent: Erase
	if (parent)
	{
		auto it = std::find(parent->children.begin(), parent->children.end(), this);
		if (it != parent->children.end()) parent->children.erase(it);
	}

	//Remove ref from children: Unparent
	for (Transform* t : children) t->parent = nullptr;
}

bool Transform::isDirty() const
{
	return isDirtySelf || (parent && parent->isDirty());
}

void Transform::markDirty()
{
	isDirtySelf = true;
}

void Transform::recompute() const
{
	//Use locals (and parent's globals) to update globals
	if (parent)
	{
		parent->ensureUpToDate();
		global.position = parent->transformPoint(local.position);
	}
	else
	{
		global = local;
	}
}

void Transform::ensureUpToDate() const
{
	if (isDirty()) recompute();
}

Transform* Transform::getParent() const
{
	return parent;
}

void Transform::setParent(Transform* newParent)
{
	assert(parent != this);

	//Setting same parent is a no-op
	if (parent == newParent) return;

	//Ensure globals are up to date, since these will be our basis
	ensureUpToDate();

	//Update parent hierarchy
	if (parent)
	{
		auto it = std::find(parent->children.begin(), parent->children.end(), this);
		assert(it != parent->children.end());
		parent->children.erase(it);
	}
	if (newParent)
	{
		assert(std::find(newParent->children.begin(), newParent->children.end(), this) == newParent->children.end());
		newParent->children.push_back(this);
	}
	parent = newParent;

	//Use globals to update locals
	if (parent)
	{
		parent->ensureUpToDate();
		local.position = parent->inverseTransformPoint(global.position);
	}
	else
	{
		global = local;
	}
}

Vector3<float> Transform::getPosition() const
{
	ensureUpToDate();
	return global.position;
}

void Transform::setPosition(Vector3<float> newPos)
{
	ensureUpToDate();
	global.position = newPos;
	if (parent) local.position = parent->inverseTransformPoint(newPos);
	else        local.position = newPos;
	markDirty(); //FIXME Should we mark children instead, since we're setting global?
}

Vector3<float> Transform::transformPoint(Vector3<float> val) const
{
	ensureUpToDate();
	return val + global.position;
}

Vector3<float> Transform::transformVector(Vector3<float> val) const
{
	return val; //TODO stub, revisit for scale/orientation
}

Vector3<float> Transform::transformNormal(Vector3<float> val) const
{
	return val; //TODO stub, revisit for scale/orientation
}

Vector3<float> Transform::inverseTransformPoint(Vector3<float> val) const
{
	ensureUpToDate();
	return val - global.position;
}

Vector3<float> Transform::inverseTransformVector(Vector3<float> val) const
{
	return val; //TODO stub, revisit for scale/orientation
}

Vector3<float> Transform::inverseTransformNormal(Vector3<float> val) const
{
	return val; //TODO stub, revisit for scale/orientation
}
