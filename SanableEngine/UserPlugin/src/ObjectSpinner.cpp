#include "ObjectSpinner.hpp"

ObjectSpinner::ObjectSpinner()
{
}

ObjectSpinner::ObjectSpinner(glm::quat spin) :
	spin(spin)
{
}

void ObjectSpinner::Update()
{
	Transform* t = gameObject->getTransform();
	t->setRotation(spin * t->getRotation());
}
