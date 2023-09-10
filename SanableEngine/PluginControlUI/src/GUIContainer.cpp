#include "GUIContainer.hpp"

#include <cassert>
#include <sstream>

#include <InterfaceEvents.h>
#include <EventSystem.h>

#include "GraphicsSystem.h"

GUI::Container::Container(const ControlID& theID, const Vector3<float>& relativePosition, const Vector3<float>& size) :
	Element(theID, relativePosition, size)
{
}

GUI::Container::Container(const ControlID& theID, const Vector3<float>& relativePosition, const Vector3<float>& size, std::initializer_list<Element*> children) :
	Container(theID, relativePosition, size)
{
	addChildren(children);
}

GUI::Container::~Container()
{
	removeAllChildren();
}

void GUI::Container::addChild(Element* child)
{
	//Make sure we don't already have it
	assert(std::find(children.begin(), children.end(), child) == children.end());
			
	//Add to children
	children.push_back(child);

	//Ensure good hierarchy
	child->setParent(this);
}

void GUI::Container::removeAllChildren()
{
	for (Element* i : children) delete i;

	children.clear();
}

void GUI::Container::tick(System* system)
{
	if (isActive())
	{
		for (Element* i : children) i->tick(system);
	}
}

void GUI::Container::draw(GraphicsBuffer* dest)
{
	for (Element* i : children) i->draw(dest);
}

void GUI::Container::rebuildAbsoluteTransform(const Vector3<float>& parentAbsPos)
{
	Element::rebuildAbsoluteTransform(parentAbsPos);
	rebuildChildAbsoluteTransforms();
}

void GUI::Container::rebuildRenderTransform(const Vector3<float>& rootRenderPos)
{
	Element::rebuildRenderTransform(rootRenderPos);
	rebuildChildRenderTransforms();
}

void GUI::Container::rebuildChildAbsoluteTransforms()
{
	for (Element* i : children) i->rebuildAbsoluteTransform(cachedAbsolutePosition);
}

void GUI::Container::rebuildChildRenderTransforms()
{
	for (Element* i : children) i->rebuildRenderTransform(cachedRenderPosition);
}

void GUI::Container::visit(const std::function<void(Element*)>& visitor)
{
	Element::visit(visitor);
	for (Element* i : children) i->visit(visitor);
}
