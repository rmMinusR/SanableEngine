#pragma once

#include "Vector3.inl"

class Transform
{
private:
	Vector3<float> position;

public:
	Transform();
	Transform(Vector3<float> position);
	Transform(float x, float y, float z);
	~Transform();

	Transform(const Transform& cpy) = delete; //Should be passed by reference - prevent accidental copies
	Transform(Transform&& mov) = default; //Don't block passing through ctors

	//Getters and setters
	inline Vector3<float> getPosition() { return position; }
	inline void setPosition(Vector3<float> v) { position = v; }
	inline void setPosition(float x, float y, float z) { position.set(x, y, z); }
};
