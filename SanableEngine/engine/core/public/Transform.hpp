#pragma once

#include <vector>

#include "dllapi.h"

#include "Vector3.inl"

class Transform
{
private:
	Transform* parent;
	std::vector<Transform*> children;
	struct Data
	{
		Vector3<float> position = Vector3<float>(0, 0, 0);
	};
	Data local;

	//Flyweight
	mutable bool isDirtySelf;
	bool isDirty() const;
	void markDirty();
	void recompute() const; //Self as well as every parent
	void ensureUpToDate() const;
	mutable Data global; //Cached

public:
	Transform();
	~Transform();

	//Ban copy/move (for now - TODO)
	Transform(const Transform&) = delete;
	Transform(Transform&&) = delete;
	Transform& operator=(const Transform&) = delete;
	Transform& operator=(Transform&&) = delete;

	//Getters and setters

	ENGINECORE_API Transform* getParent() const;
	ENGINECORE_API void setParent(Transform*);

	ENGINECORE_API Vector3<float> getPosition() const;
	ENGINECORE_API void setPosition(Vector3<float>);

	//Utility functions

	//Local to global
	ENGINECORE_API Vector3<float> transformPoint(Vector3<float>) const;
	ENGINECORE_API Vector3<float> transformVector(Vector3<float>) const;
	ENGINECORE_API Vector3<float> transformNormal(Vector3<float>) const;

	//Global to local
	ENGINECORE_API Vector3<float> inverseTransformPoint(Vector3<float>) const;
	ENGINECORE_API Vector3<float> inverseTransformVector(Vector3<float>) const;
	ENGINECORE_API Vector3<float> inverseTransformNormal(Vector3<float>) const;
};
