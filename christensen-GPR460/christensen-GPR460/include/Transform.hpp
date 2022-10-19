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
	inline float getX() const { return x; }
	inline float getY() const { return y; }
	inline float getZ() const { return z; }
	inline void setX(float v) { x = v; }
	inline void setY(float v) { y = v; }
	inline void setZ(float v) { z = v; }

	inline void set(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};
