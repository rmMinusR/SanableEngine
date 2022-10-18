#include "Transform.hpp"

Transform::Transform() : Transform(0, 0, 0)
{
}

Transform::Transform(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Transform::~Transform()
{
}
