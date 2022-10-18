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
	float getX() inline const { return x; }
	float getX() inline const { return y; }
	float getX() inline const { return z; }
	void setX(float v) inline { x = v; }
	void setY(float v) inline { y = v; }
	void setZ(float v) inline { z = v; }
};
