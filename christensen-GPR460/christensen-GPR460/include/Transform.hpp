#pragma once

class Transform
{
private:
	float x, y, z;

public:
	Transform();
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
};
