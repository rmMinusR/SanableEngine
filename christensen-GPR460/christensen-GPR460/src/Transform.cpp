#include "Transform.hpp"

Transform::Transform() : Transform(0, 0, 0)
{
}

Transform::Transform(Vector3<float> position) :
	position(position)
{
}

Transform::Transform(float x, float y, float z) :
	position(x, y, z)
{
}

Transform::~Transform()
{
}
